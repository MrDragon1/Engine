#include "BitangentNode.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {

BitangentNode::BitangentNode() {}

ShaderNodeImplPtr BitangentNode::Create() { return BitangentNodePtr::Create(); }

void BitangentNode::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                           ShaderStagePtr stage) {
    auto& shaderGen = context->GetShaderGenerator();

    if (stage->GetName() == Stage::VERTEX) {
        VariableBlock& vertexData = stage->GetOutputBlock(ShaderBuildInVariable::VERTEXDATA);
        auto bitangent = vertexData[ShaderBuildInVariable::BITANGENT_WORLD];
        if (!bitangent->IsEmitted()) {
            bitangent->SetEmitted();
            auto normal = vertexData[ShaderBuildInVariable::NORMAL_WORLD];
            if (!normal->IsEmitted()) {
                normal->SetEmitted();
                stage->EmitLine(normal->GetVariable(context->GetScope()) + " = normalize((" +
                                ShaderBuildInVariable::MODEL_INVERSE_TRANSPOSE_MATRIX + " * vec4(" +
                                ShaderBuildInVariable::IN_NORMAL + ", 0.0)).xyz);");
            }
            auto tangent = vertexData[ShaderBuildInVariable::TANGENT_WORLD];
            if (!tangent->IsEmitted()) {
                tangent->SetEmitted();
                stage->EmitLine(tangent->GetVariable(context->GetScope()) + " = normalize((" +
                                ShaderBuildInVariable::MODEL_MATRIX + " * vec4(" +
                                ShaderBuildInVariable::IN_TANGENT + ", 0.0)).xyz);");
            }
            stage->EmitLine(bitangent->GetVariable(context->GetScope()) + " = cross(" +
                            normal->GetVariable(context->GetScope()) + ", " +
                            tangent->GetVariable(context->GetScope()) + ");");
        }
    }
    if (stage->GetName() == Stage::PIXEL) {
        VariableBlock& vertexData = stage->GetInputBlock(ShaderBuildInVariable::VERTEXDATA);

        auto bitangent = vertexData[ShaderBuildInVariable::BITANGENT_WORLD];
        stage->EmitLine(node->GetOutput()->GetValue()->GetSyntaxString() + " " +
                        node->GetOutput()->GetVariable(context->GetScope()) + " = " +
                        bitangent->GetVariable(context->GetScope()) + ";");
    }
}

void BitangentNode::CreateVariables(ShaderNodePtr node, ShaderContextPtr context,
                                          ShaderPtr shader) {
    ShaderStagePtr vs = shader->GetStage(Stage::VERTEX);
    ShaderStagePtr ps = shader->GetStage(Stage::PIXEL);

    vs->GetInputBlock(ShaderBuildInVariable::INPUT)
        .Add(nullptr, "float3", ShaderBuildInVariable::IN_BITANGENT);

    auto& shaderGen = context->GetShaderGenerator();
    shaderGen.AddConnectorVariable(ShaderBuildInVariable::VERTEXDATA, "float3",
                                   ShaderBuildInVariable::BITANGENT_WORLD, vs, ps);

    VariableBlockPtr vsPublicUniforms = vs->CreateUniformBlock(ShaderBuildInVariable::VSPUBUNIFORM);
    vsPublicUniforms->Add(nullptr, "matrix4", ShaderBuildInVariable::MODEL_MATRIX);
}

}  // namespace Ethereal
