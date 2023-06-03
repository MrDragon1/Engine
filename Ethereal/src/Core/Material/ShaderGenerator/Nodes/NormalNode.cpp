#include "NormalNode.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {

NormalNode::NormalNode() {}

ShaderNodeImplPtr NormalNode::Create() { return NormalNodePtr::Create(); }

void NormalNode::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                  ShaderStagePtr stage) {
    auto& shaderGen = context->GetShaderGenerator();

    if (stage->GetName() == Stage::VERTEX) {
        VariableBlock& vertexData = stage->GetOutputBlock(ShaderBuildInVariable::VERTEXDATA);
        auto normal = vertexData[ShaderBuildInVariable::NORMAL_WORLD];
        if (!normal->IsEmitted()) {
            normal->SetEmitted();
            stage->EmitLine(normal->GetVariable(context->GetScope()) + " = normalize((" +
                            ShaderBuildInVariable::MODEL_INVERSE_TRANSPOSE_MATRIX + " * vec4(" +
                            ShaderBuildInVariable::IN_NORMAL + ", 0.0)).xyz);");
        }
    }
    if (stage->GetName() == Stage::PIXEL) {
        VariableBlock& vertexData = stage->GetInputBlock(ShaderBuildInVariable::VERTEXDATA);

        auto normal = vertexData[ShaderBuildInVariable::NORMAL_WORLD];
        stage->EmitLine(node->GetOutput()->GetValue()->GetSyntaxString() + " " +
                        node->GetOutput()->GetVariable(context->GetScope()) + " = " +
                        normal->GetVariable(context->GetScope()) + ";");
    }
}

void NormalNode::CreateVariables(ShaderNodePtr node, ShaderContextPtr context, ShaderPtr shader) {
    ShaderStagePtr vs = shader->GetStage(Stage::VERTEX);
    ShaderStagePtr ps = shader->GetStage(Stage::PIXEL);

    vs->GetInputBlock(ShaderBuildInVariable::INPUT)
        .Add(nullptr, "float3", ShaderBuildInVariable::IN_NORMAL);

    auto& shaderGen = context->GetShaderGenerator();
    shaderGen.AddConnectorVariable(ShaderBuildInVariable::VERTEXDATA, "float3",
                                   ShaderBuildInVariable::NORMAL_WORLD, vs, ps);

    VariableBlockPtr vsPublicUniforms = vs->CreateUniformBlock(ShaderBuildInVariable::VSPUBUNIFORM);
    vsPublicUniforms->Add(nullptr, "matrix4",
                          ShaderBuildInVariable::MODEL_INVERSE_TRANSPOSE_MATRIX);
}

}  // namespace Ethereal
