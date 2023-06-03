#include "ShaderGenerator.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
#include "Base/GlobalContext.h"
#include "Core/Material/MaterialNode.h"
#include "Utils/StringUtils.h"
#include "Core/Material/ShaderGenerator/Nodes/SourceCodeNode.h"
#include "Core/Material/ShaderGenerator/Nodes/AggregationNode.h"
#include "Core/Material/ShaderGenerator/Nodes/CompoundNode.h"
#include "Core/Material/ShaderGenerator/Nodes/ClosureCompoundNode.h"
#include "Core/Material/ShaderGenerator/Nodes/PositionNode.h"
#include "Core/Material/ShaderGenerator/Nodes/NormalNode.h"
#include "Core/Material/ShaderGenerator/Nodes/TangentNode.h"
#include "Core/Material/ShaderGenerator/Nodes/BitangentNode.h"
#include "Core/Material/ShaderGenerator/Nodes/ClosureMixNode.h"
#include "Core/Material/ShaderGenerator/Nodes/ClosureMultiplyNode.h"
#include "Core/Material/ShaderGenerator/Nodes/ClosureAddNode.h"
#include "Core/Material/ShaderGenerator/Nodes/SurfaceNode.h"
namespace Ethereal {
ShaderStagePtr Shader::CreateStage(const string& name) {
    auto it = mStages.find(name);
    if (it == mStages.end()) {
        mStages[name] = ShaderStagePtr::Create(name);
    }
    return mStages[name];
}

ShaderStagePtr Shader::GetStage(const string& name) {
    auto it = mStages.find(name);
    if (it == mStages.end()) {
        return nullptr;
    }
    return mStages[name];
}

void Shader::Compile() {
    ShaderSourceString sss;
    if (GetStage(Stage::VERTEX)) sss[ShaderType::VERTEX] = GetStage(Stage::VERTEX)->GetSourceCode();
    if (GetStage(Stage::PIXEL)) sss[ShaderType::FRAGMENT] = GetStage(Stage::PIXEL)->GetSourceCode();

    auto api = GlobalContext::GetDriverApi();
    mProgram = api->CreateProgram("MAT_" + mName, sss);
}

ShaderGenerator::ShaderGenerator() {
    StringVec elementNames;
    RegisterImplementation("IM_position_float3", PositionNode::Create);
    RegisterImplementation("IM_normal_float3", NormalNode::Create);
    RegisterImplementation("IM_tangent_float3", TangentNode::Create);
    RegisterImplementation("IM_bitangent_float3", BitangentNode::Create);

    RegisterImplementation("IM_surface_surfaceshader", SurfaceNode::Create);

    // <!-- <mix> -->
    RegisterImplementation("IM_mix_bsdf", ClosureMixNode::Create);
    RegisterImplementation("IM_mix_edf", ClosureMixNode::Create);
    // <!-- <add> -->
    RegisterImplementation("IM_add_bsdf", ClosureAddNode::Create);
    RegisterImplementation("IM_add_edf", ClosureAddNode::Create);
    // <!-- <multiply> -->
    elementNames = {
        "IM_multiply_bsdfC",
        "IM_multiply_bsdfF",
        "IM_multiply_edfC_",
        "IM_multiply_edfF",
    };
    RegisterImplementation(elementNames, ClosureMultiplyNode::Create);
}

void ShaderGenerator::CreateVariables(ShaderGraphPtr graph, ShaderContextPtr context,
                                      ShaderPtr shader) {
    for (auto& node : graph->GetSortedNodes()) {
        node->GetImpl()->CreateVariables(node, context, shader);
    }
}

void ShaderGenerator::EmitVertexStage(ShaderGraphPtr graph, ShaderContextPtr context,
                                      ShaderStagePtr stage) {
    EmitVersion(context, stage);

    EmitTypeDefinition(context, stage);

    EmitUniforms(context, stage);

    EmitInputs(context, stage);

    EmitOutputs(context, stage);

    EmitLibs(context, stage);

    EmitFunctionDefinitions(graph, context, stage);

    stage->EmitLine("void main()");
    stage->BeginScope();
    auto& block = stage->GetUniformBlock(ShaderBuildInVariable::VSPUBUNIFORM);

    stage->EmitLine("vec4 hPositionWorld = " +
                    block[ShaderBuildInVariable::MODEL_MATRIX]->GetVariable(context->GetScope()) +
                    " * vec4(i_position, 1.0);");
    stage->EmitLine("gl_Position = " +
                    block[ShaderBuildInVariable::PROJ_MATRIX]->GetVariable(context->GetScope()) +
                    " * " +
                    block[ShaderBuildInVariable::VIEW_MATRIX]->GetVariable(context->GetScope()) +
                    " * hPositionWorld;");

    for (auto node : graph->GetSortedNodes()) {
        stage->AddFunctionCall(node, context);
    }
    stage->EndScope();
}

void ShaderGenerator::EmitPixelStage(ShaderGraphPtr graph, ShaderContextPtr context,
                                     ShaderStagePtr stage) {
    EmitVersion(context, stage);

    EmitTypeDefinition(context, stage);

    EmitUniforms(context, stage);

    EmitInputs(context, stage);

    EmitOutputs(context, stage);

    EmitLibs(context, stage);

    EmitLightData(context, stage);

    EmitFunctionDefinitions(graph, context, stage);

    stage->EmitLine("void main()");
    stage->BeginScope();

    EmitFunctionCalls(graph, context, stage);

    for (auto& [name, output] : graph->GetOutputSockets()) {
        if (output->GetValue()->Is<material>()) {
            stage->EmitLine(output->GetVariable(context->GetScope()) + " = vec4(" +
                            output->GetConnector()->GetVariable(context->GetScope()) +
                            ".color, 1.0);");
        } else {
            stage->EmitLine(output->GetVariable(context->GetScope()) + " = " +
                            output->GetConnector()->GetVariable(context->GetScope()) + ";");
        }
    }

    stage->EndScope();
}

void ShaderGenerator::AddConnectorBlock(const string& name, const string& instance,
                                        ShaderStagePtr src, ShaderStagePtr dst) {
    src->CreateOutputBlock(name, instance);
    dst->CreateInputBlock(name, instance);
}

void ShaderGenerator::AddConnectorVariable(const string& name, const string& type,
                                           const string& variable, ShaderStagePtr src,
                                           ShaderStagePtr dst) {
    VariableBlock& vsConn = src->GetOutputBlock(name);
    VariableBlock& psConn = dst->GetInputBlock(name);
    vsConn.Add(nullptr, type, variable);
    psConn.Add(nullptr, type, variable);
}

void ShaderGenerator::EmitTypeDefinition(ShaderContextPtr context, ShaderStagePtr stage) {
    ValueBase::EmitTypeDefine(context, stage);
}

void ShaderGenerator::EmitFunctionDefinition(ShaderNodePtr node, ShaderContextPtr context,
                                             ShaderStagePtr stage) {
    stage->AddFunctionDefinition(node, context);
}

void ShaderGenerator::EmitFunctionDefinitions(ShaderGraphPtr graph, ShaderContextPtr context,
                                              ShaderStagePtr stage) {
    for (auto& node : graph->GetSortedNodes()) {
        EmitFunctionDefinition(node, context, stage);
    }
}

void ShaderGenerator::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                       ShaderStagePtr stage, bool checkScope /*= true*/) {
    stage->AddFunctionCall(node, context);
}

void ShaderGenerator::EmitFunctionCalls(ShaderGraphPtr graph, ShaderContextPtr context,
                                        ShaderStagePtr stage, uint32_t classification /*= 0u*/) {
    for (auto node : graph->GetSortedNodes()) {
        if (!classification || node->HasClassification(classification)) {
            EmitFunctionCall(node, context, stage);
        }
    }
}

void ShaderGenerator::EmitDependentFunctionCalls(ShaderNodePtr node, ShaderContextPtr context,
                                                 ShaderStagePtr stage, uint32_t classification) {
    for (auto& [name, input] : node->GetInputs()) {
        ShaderNodePtr upstream = input->GetSibling();
        if (upstream && (!classification || upstream->HasClassification(classification))) {
            EmitFunctionCall(upstream, context, stage);
        }
    }
}

void ShaderGenerator::EmitLightData(ShaderContextPtr context, ShaderStagePtr stage) {
    stage->EmitLine("#define " + ShaderBuildInVariable::LIGHT_DATA_MAX_LIGHT_SOURCES + " 5");
    stage->EmitLine();

    VariableBlock& lightData = stage->GetUniformBlock(ShaderBuildInVariable::LIGHT_DATA);
    string structArraySuffix = "[" + ShaderBuildInVariable::LIGHT_DATA_MAX_LIGHT_SOURCES + "]";
    string structName = lightData.GetInstance();

    stage->EmitLine();
    stage->EmitLine("struct " + lightData.GetName());
    stage->BeginScope();

    stage->EmitLine("int type;");
    stage->EmitLine("vec3 direction;");
    stage->EmitLine("vec3 color;");
    stage->EmitLine("float intensity;");

    stage->EndScope(";");
    stage->EmitLine();
    stage->EmitLine("uniform " + lightData.GetName() + " " + structName + structArraySuffix + ";");

    stage->EmitLine();
    stage->EmitFile("assets/materials/lib/glsl/light.glsl");
    stage->EmitLine();
}

ShaderNodeImplPtr ShaderGenerator::GetImpl(NodeInstancePtr instance, ShaderContextPtr context) {
    NodeImplPtr implElem = instance->GetNodeImpl();
    ShaderNodeImplPtr impl = nullptr;
    if (implElem && implElem->IsNodeGraph()) {
        if (instance->GetOutputs()[0]->IsClosure()) {
            impl = ClosureCompoundNode::Create();
        } else {
            impl = CompoundNode::Create();
        }
    } else if (implElem && implElem->IsDynamic()) {
        impl = mImplFactory.create(implElem->GetName());
    } else {
        impl = SourceCodeShaderNode::Create();
    }
    impl->Initilize(implElem, context);
    return impl;
}

void ShaderGenerator::EmitUniforms(ShaderContextPtr context, ShaderStagePtr stage) {
    if (stage->GetName() == Stage::VERTEX) {
        for (auto& it : stage->GetUniformBlocks()) {
            VariableBlock& uniforms = *it.second;
            string instance = it.second->GetInstance();
            /// glGetUniformLocation not support structure in glsl
            // stage->EmitLine("uniform " + uniforms.GetName());
            // stage->BeginScope();
            for (auto& uniform : uniforms.GetVariables()) {
                stage->EmitVariableDeclaration(uniform, context, "uniform");
                // uniform->SetVariable(instance + "." + uniform->GetVariable(context->GetScope()));
            }
            // stage->EndScope(" " + instance + ";");
        }
        stage->EmitLine();
    }
    if (stage->GetName() == Stage::PIXEL) {
        for (auto& it : stage->GetUniformBlocks()) {
            VariableBlock& uniforms = *it.second;
            string instance = it.second->GetInstance();
            // stage->EmitLine("uniform " + uniforms.GetName());
            // stage->BeginScope();
            for (auto& uniform : uniforms.GetVariables()) {
                stage->EmitVariableDeclaration(uniform, context, "uniform");
                // uniform->SetVariable(instance + "." + uniform->GetVariable(context->GetScope()));
            }
            // stage->EndScope(" " + instance + ";");
        }
        stage->EmitLine();
    }
}

void ShaderGenerator::EmitOutputs(ShaderContextPtr context, ShaderStagePtr stage) {
    if (stage->GetName() == Stage::VERTEX) {
        for (auto& it : stage->GetOutputBlocks()) {
            stage->EmitLine("out " + it.second->GetName());
            stage->BeginScope();
            VariableBlock& outputs = *it.second;
            string instance = it.second->GetInstance();
            for (auto& output : outputs.GetVariables()) {
                stage->EmitVariableDeclaration(output, context);
                output->SetVariable(instance + "." + output->GetVariable(context->GetScope()));
            }
            stage->EndScope(" " + instance + ";");
        }
        stage->EmitLine();
    }
    if (stage->GetName() == Stage::PIXEL) {
        stage->EmitComment("Pixel Shader Output");
        for (auto& it : stage->GetOutputBlocks()) {
            VariableBlock& outputs = *it.second;
            size_t location = 0;
            for (auto& output : outputs.GetVariables()) {
                stage->EmitVariableDeclaration(
                    output, context, "layout(location = " + std::to_string(location++) + ") out");
            }
        }
        stage->EmitLine();
    }
}

void ShaderGenerator::EmitInputs(ShaderContextPtr context, ShaderStagePtr stage) {
    if (stage->GetName() == Stage::VERTEX) {
        // for (auto& it : stage->GetInputBlocks()) {
        //     VariableBlock& inputs = *it.second;
        //     size_t location = 0;
        //     for (auto input : inputs.GetVariables()) {
        //         stage->EmitVariableDeclaration(
        //             input, context, "layout(location = " + std::to_string(location++) + " ) in",
        //             false);
        //     }
        // }
        // TODO: emit only necessary inputs
        string expression =
            "layout(location = 0) in vec3 i_position;\n"
            "layout(location = 1) in vec3 i_normal;\n"
            "layout(location = 2) in vec3 i_tangent;\n"
            "layout(location = 3) in vec3 i_bitangent;\n"
            "layout(location = 4) in vec2 i_texcoord;\n";
        stage->EmitLine(expression);
        stage->EmitLine();
    }
    if (stage->GetName() == Stage::PIXEL) {
        for (auto& it : stage->GetInputBlocks()) {
            stage->EmitLine("in " + it.second->GetName());
            stage->BeginScope();
            VariableBlock& inputs = *it.second;
            string instance = it.second->GetInstance();
            for (auto input : inputs.GetVariables()) {
                stage->EmitVariableDeclaration(input, context);
                input->SetVariable(instance + "." + input->GetVariable(context->GetScope()));
            }
            stage->EndScope(" " + instance + ";");
        }
        stage->EmitLine();
    }
}

void ShaderGenerator::EmitVersion(ShaderContextPtr context, ShaderStagePtr stage) {
    stage->EmitLine("#version 460");
    stage->EmitLine();
}

void ShaderGenerator::EmitLibs(ShaderContextPtr context, ShaderStagePtr stage) {
    if (stage->GetName() == Stage::VERTEX) {
    }
    if (stage->GetName() == Stage::PIXEL) {
        stage->EmitFile("assets/materials/lib/glsl/base.glsl");
    }
}

void ShaderGenerator::RegisterImplementation(const string& name,
                                             CreatorFunction<ShaderNodeImpl> func) {
    mImplFactory.registerClass(name, func);
}

void ShaderGenerator::RegisterImplementation(const StringVec& nameVec,
                                             CreatorFunction<ShaderNodeImpl> func) {
    for (auto& name : nameVec) {
        RegisterImplementation(name, func);
    }
}

}  // namespace Ethereal
