#include "ShaderGenerator.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
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

    stage->EmitLine("vec4 hPositionWorld = " + ShaderBuildInVariable::MODEL_MATRIX +
                    " * vec4(i_position, 1.0);");
    stage->EmitLine("gl_Position = " + ShaderBuildInVariable::PROJ_MATRIX + " * " +
                    ShaderBuildInVariable::VIEW_MATRIX + " * hPositionWorld;");

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
            for (auto uniform : uniforms.GetVariables()) {
                stage->EmitVariableDeclaration(uniform, context, "uniform");
            }
        }
        stage->EmitLine();
    }
    if (stage->GetName() == Stage::PIXEL) {
        for (auto& it : stage->GetUniformBlocks()) {
            VariableBlock& uniforms = *it.second;
            for (auto uniform : uniforms.GetVariables()) {
                stage->EmitVariableDeclaration(uniform, context, "uniform", true);
            }
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
            for (auto& output : outputs.GetVariables()) {
                stage->EmitVariableDeclaration(output, context);
            }
            stage->EndScope(" " + it.second->GetInstance() + ";");
        }
        stage->EmitLine();
    }
    if (stage->GetName() == Stage::PIXEL) {
        stage->EmitComment("Pixel Shader Output");
        for (auto& it : stage->GetOutputBlocks()) {
            VariableBlock& outputs = *it.second;
            for (auto& output : outputs.GetVariables()) {
                stage->EmitVariableDeclaration(output, context, "out");
            }
        }
        stage->EmitLine();
    }
}

void ShaderGenerator::EmitInputs(ShaderContextPtr context, ShaderStagePtr stage) {
    if (stage->GetName() == Stage::VERTEX) {
        for (auto& it : stage->GetInputBlocks()) {
            VariableBlock& inputs = *it.second;
            for (auto input : inputs.GetVariables()) {
                stage->EmitVariableDeclaration(input, context, "in", false);
            }
        }
        stage->EmitLine();
    }
    if (stage->GetName() == Stage::PIXEL) {
        for (auto& it : stage->GetInputBlocks()) {
            stage->EmitLine("in " + it.second->GetName());
            stage->BeginScope();
            VariableBlock& inputs = *it.second;
            for (auto input : inputs.GetVariables()) {
                stage->EmitVariableDeclaration(input, context);
            }
            stage->EndScope(" " + it.second->GetInstance() + ";");
        }
        stage->EmitLine();
    }
}

void ShaderGenerator::EmitVersion(ShaderContextPtr context, ShaderStagePtr stage) {
    stage->EmitLine("#version 460");
    stage->EmitLine();
}

}  // namespace Ethereal
