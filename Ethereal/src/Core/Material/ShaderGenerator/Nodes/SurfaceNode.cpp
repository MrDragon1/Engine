#include "SurfaceNode.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {
SurfaceNode::SurfaceNode() {}

ShaderNodeImplPtr SurfaceNode::Create() { return SurfaceNodePtr::Create(); }

void SurfaceNode::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                   ShaderStagePtr stage) {
    ShaderGenerator& generator = context->GetShaderGenerator();

    if (stage->GetName() == Stage::VERTEX) {
        VariableBlock& vertexData = stage->GetOutputBlock(ShaderBuildInVariable::VERTEXDATA);
        auto position = vertexData[ShaderBuildInVariable::POSITION_WORLD];
        if (!position->IsEmitted()) {
            position->SetEmitted();
            stage->EmitLine(position->GetVariable(context->GetScope()) + " = hPositionWorld.xyz");
        }
        auto normal = vertexData[ShaderBuildInVariable::NORMAL_WORLD];
        if (!normal->IsEmitted()) {
            normal->SetEmitted();
            stage->EmitLine(normal->GetVariable(context->GetScope()) + " = normalize((" +
                            ShaderBuildInVariable::MODEL_INVERSE_TRANSPOSE_MATRIX + " * vec4(" +
                            ShaderBuildInVariable::IN_NORMAL + ", 0)).xyz)");
        }
    } else if (stage->GetName() == Stage::PIXEL) {
        VariableBlock& vertexData = stage->GetInputBlock(ShaderBuildInVariable::VERTEXDATA);
        auto output = node->GetOutput("out1");
        stage->EmitVariableDeclaration(output, context, "", true);
        stage->BeginScope();

        const string outColor = output->GetVariable(context->GetScope()) + ".color";
        const string outTransparency = output->GetVariable(context->GetScope()) + ".transparency";

        auto bsdfInput = node->GetInput("bsdf");
        ShaderNodePtr bsdf = bsdfInput->GetConnector()->GetParent();
        if (bsdf) {
            EmitLightLoop(node, context, stage, outColor);
        }

        stage->EndScope();
    }
}

void SurfaceNode::CreateVariables(ShaderNodePtr node, ShaderContextPtr context, ShaderPtr shader) {}

void SurfaceNode::EmitLightLoop(ShaderNodePtr node, ShaderContextPtr context, ShaderStagePtr stage,
                                const string outColor) {
    ShaderGenerator& generator = context->GetShaderGenerator();
    VariableBlock& vertexData = stage->GetInputBlock(ShaderBuildInVariable::VERTEXDATA);
    auto bsdfInput = node->GetInput("bsdf");
    ShaderNodePtr bsdf = bsdfInput->GetConnector()->GetParent();

    stage->EmitComment("Light Loop");
    stage->EmitLine("int numLights = numActiveLightSources();");
    stage->EmitLine("lightshader lightShader;");
    stage->EmitLine(
        "for (int activeLightIndex = 0; activeLightIndex < numLights; ++activeLightIndex)");

    stage->BeginScope();
    stage->EmitLine(
        "sampleLightSource(" + ShaderBuildInVariable::LIGHT_DATA_INSTANCE + "[activeLightIndex], " +
        vertexData[ShaderBuildInVariable::POSITION_WORLD]->GetVariable(context->GetScope()) +
        ", lightShader);");
    stage->EmitLine("L = lightShader.direction;");

    stage->EmitComment("Calculate the BSDF response for this light source");
    generator.EmitFunctionCall(bsdf, context, stage);

    stage->EmitLine();
    stage->EmitComment("Accumulate the light's contribution");
    stage->EmitLine(outColor + " += lightShader.intensity * " +
                    bsdf->GetOutput("out1")->GetVariable(context->GetScope()) + ".response;");

    stage->EndScope();
}

}  // namespace Ethereal
