#include "ShaderGenerator.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
#include "Base/GlobalContext.h"
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

void ShaderGenerator::CreateVariables(ShaderGraphPtr graph, ShaderContextPtr context,
                                      ShaderPtr shader) {
    for (auto& node : graph->GetSortedNodes()) {
        node->GetImpl()->CreateVariables(node, context, shader);
    }
}

void ShaderGenerator::EmitVertexStage(ShaderGraphPtr graph, ShaderContextPtr context,
                                      ShaderStagePtr stage) {
    EmitVersion(context, stage);

    ValueBase::EmitTypeDefine(context, stage);

    EmitUniforms(context, stage);

    EmitInputs(context, stage);

    EmitOutputs(context, stage);

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

    ValueBase::EmitTypeDefine(context, stage);

    EmitUniforms(context, stage);

    EmitInputs(context, stage);

    EmitOutputs(context, stage);

    for (auto& node : graph->GetSortedNodes()) {
        stage->AddFunctionDefinition(node, context);
    }

    stage->EmitLine("void main()");
    stage->BeginScope();
    for (auto node : graph->GetSortedNodes()) {
        stage->AddFunctionCall(node, context);
    }

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

void ShaderGenerator::EmitUniforms(ShaderContextPtr context, ShaderStagePtr stage) {
    if (stage->GetName() == Stage::VERTEX) {
        for (auto& it : stage->GetUniformBlocks()) {
            VariableBlock& uniforms = *it.second;
            string instance = it.second->GetInstance();
            stage->EmitLine("layout(std140, binding = 0) uniform " + uniforms.GetName());
            stage->BeginScope();
            for (auto& uniform : uniforms.GetVariables()) {
                stage->EmitVariableDeclaration(uniform, context);
                uniform->SetVariable(instance + "." + uniform->GetVariable(context->GetScope()));
            }
            stage->EndScope(" " + instance + ";");
        }
        stage->EmitLine();
    }
    if (stage->GetName() == Stage::PIXEL) {
        for (auto& it : stage->GetUniformBlocks()) {
            VariableBlock& uniforms = *it.second;
            string instance = it.second->GetInstance();
            stage->EmitLine("layout(std140, binding = " + std::to_string(uniforms.GetBinding()) +
                            ") uniform " + uniforms.GetName());
            stage->BeginScope();
            for (auto& uniform : uniforms.GetVariables()) {
                stage->EmitVariableDeclaration(uniform, context);
                uniform->SetVariable(instance + "." + uniform->GetVariable(context->GetScope()));
            }
            stage->EndScope(" " + instance + ";");
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
        for (auto& it : stage->GetInputBlocks()) {
            VariableBlock& inputs = *it.second;
            size_t location = 0;
            for (auto input : inputs.GetVariables()) {
                stage->EmitVariableDeclaration(
                    input, context, "layout(location = " + std::to_string(location++) + " ) in",
                    false);
            }
        }
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

}  // namespace Ethereal
