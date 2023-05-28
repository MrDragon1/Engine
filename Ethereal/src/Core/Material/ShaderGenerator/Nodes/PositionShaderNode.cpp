#include "PositionShaderNode.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {

PositionShaderNode::PositionShaderNode() {}

ShaderNodeImplPtr PositionShaderNode::Create() { return PositionShaderNodePtr::Create(); }

void PositionShaderNode::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                          ShaderStagePtr stage) {
    auto& shaderGen = context->GetShaderGenerator();

    if (stage->GetName() == Stage::VERTEX) {
        VariableBlock& vertexData = stage->GetOutputBlock(ShaderBuildInVariable::VERTEXDATA);
        auto worldPos = vertexData[ShaderBuildInVariable::POSITION_WORLD];
        if (!worldPos->IsEmitted()) {
            worldPos->SetEmitted();
            stage->EmitLine(worldPos->GetVariable(context->GetScope()) + " = hPositionWorld.xyz;");
        }
    }
    if (stage->GetName() == Stage::PIXEL) {
        VariableBlock& vertexData = stage->GetInputBlock(ShaderBuildInVariable::VERTEXDATA);

        auto position = vertexData[ShaderBuildInVariable::POSITION_WORLD];
        stage->EmitLine(node->GetOutput()->GetValue()->GetSyntaxString() + " " +
                        node->GetOutput()->GetVariable(context->GetScope()) + " = " +
                        position->GetVariable(context->GetScope()) + ";");
    }
}

void PositionShaderNode::CreateVariables(ShaderNodePtr node, ShaderContextPtr context,
                                         ShaderPtr shader) {
    ShaderStagePtr vs = shader->GetStage(Stage::VERTEX);
    ShaderStagePtr ps = shader->GetStage(Stage::PIXEL);

    vs->GetInputBlock(ShaderBuildInVariable::INPUT)
        .Add(nullptr, "float3", ShaderBuildInVariable::IN_POSITION);

    auto& shaderGen = context->GetShaderGenerator();
    shaderGen.AddConnectorVariable(ShaderBuildInVariable::VERTEXDATA, "float3",
                                   ShaderBuildInVariable::POSITION_WORLD, vs, ps);
}

}  // namespace Ethereal
