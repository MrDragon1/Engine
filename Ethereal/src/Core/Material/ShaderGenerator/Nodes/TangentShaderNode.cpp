#include "TangentShaderNode.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {

TangentShaderNode::TangentShaderNode() {}

ShaderNodeImplPtr TangentShaderNode::Create() { return TangentShaderNodePtr::Create(); }

void TangentShaderNode::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                         ShaderStagePtr stage) {
    auto& shaderGen = context->GetShaderGenerator();

    if (stage->GetName() == Stage::VERTEX) {
        VariableBlock& vertexData = stage->GetOutputBlock(ShaderBuildInVariable::VERTEXDATA);
        auto tangent = vertexData[ShaderBuildInVariable::TANGENT_WORLD];
        if (!tangent->IsEmitted()) {
            tangent->SetEmitted();
            stage->EmitLine(tangent->GetVariable(context->GetScope()) + " = normalize((" +
                            ShaderBuildInVariable::MODEL_MATRIX + " * vec4(" +
                            ShaderBuildInVariable::IN_TANGENT + ", 0.0)).xyz);");
        }
    }
    if (stage->GetName() == Stage::PIXEL) {
        VariableBlock& vertexData = stage->GetInputBlock(ShaderBuildInVariable::VERTEXDATA);

        auto tangent = vertexData[ShaderBuildInVariable::TANGENT_WORLD];
        stage->EmitLine(node->GetOutput()->GetValue()->GetSyntaxString() + " " +
                        node->GetOutput()->GetVariable(context->GetScope()) + " = " +
                        tangent->GetVariable(context->GetScope()) + ";");
    }
}

void TangentShaderNode::CreateVariables(ShaderNodePtr node, ShaderContextPtr context,
                                        ShaderPtr shader) {
    ShaderStagePtr vs = shader->GetStage(Stage::VERTEX);
    ShaderStagePtr ps = shader->GetStage(Stage::PIXEL);

    vs->GetInputBlock(ShaderBuildInVariable::INPUT)
        .Add(nullptr, "float3", ShaderBuildInVariable::IN_TANGENT);

    auto& shaderGen = context->GetShaderGenerator();
    shaderGen.AddConnectorVariable(ShaderBuildInVariable::VERTEXDATA, "float3",
                                   ShaderBuildInVariable::TANGENT_WORLD, vs, ps);

    VariableBlockPtr vsPublicUniforms = vs->CreateUniformBlock(ShaderBuildInVariable::VSPUBUNIFORM);
    vsPublicUniforms->Add(nullptr, "matrix4", ShaderBuildInVariable::MODEL_MATRIX);
}

}  // namespace Ethereal
