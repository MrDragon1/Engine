#include "SourceCodeShaderNode.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
#include "Utils/StringUtils.h"
namespace Ethereal {
SourceCodeShaderNode::SourceCodeShaderNode(NodeImplPtr impl) : ShaderNodeImpl(impl) {
    mFunctionSource = impl->GetAttribute(MaterialAttribute::SOURCECODE);

    if (mFunctionSource.empty()) {
        // this impl is not inline,
        // turn to find sourcecode in "file" attribute
        mSourceFilename = impl->GetAttribute(MaterialAttribute::FILE);
        mFunctionSource = Utils::ReadFileAndSkipBOM(mSourceFilename);
    }

    mFunctionName = impl->GetAttribute(MaterialAttribute::FUNCTION);
    mInlined = mFunctionName.empty();
}

void SourceCodeShaderNode::EmitFunctionDefinition(ShaderNodePtr node, ShaderContextPtr context,
                                                  ShaderStagePtr stage) {
    if (stage->GetName() == Stage::VERTEX) {
    }
    if (stage->GetName() == Stage::PIXEL) {
        if (!mFunctionSource.empty() && !mInlined) {
            stage->EmitLine(mFunctionSource);
        }
    }
}

void SourceCodeShaderNode::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                            ShaderStagePtr stage) {
    auto& shaderGen = context->GetShaderGenerator();

    if (stage->GetName() == Stage::VERTEX) {
        VariableBlock& vertexData = stage->GetOutputBlock("vertexData");
        auto worldPos = vertexData[ShaderBuildInVariable::POSITION_WORLD];
        if (!worldPos->IsEmitted()) {
            worldPos->SetEmitted();
            stage->EmitLine(worldPos->GetVariable(context->GetScope()) + " = hPositionWorld.xyz;");
        }
    }
    if (stage->GetName() == Stage::PIXEL) {
        if (mInlined) {
            string sourcecode = mFunctionSource;
            StringMap map;
            for (auto& name : node->GetInputOrder()) {
                auto input = node->GetInput(name);
                auto output = input->GetConnector();
                if (output) {
                    map[name] = output->GetVariable(context->GetScope());
                } else {
                    map[name] = "Unknown value";
                }
            }
            Utils::ReplaceIdentifier(sourcecode, map);

            string expression = node->GetOutput()->GetValue()->GetSyntaxString() + " " +
                                node->GetOutput()->GetVariable(context->GetScope()) + " = " +
                                sourcecode + ";";

            stage->EmitLine(expression);
        } else {
            string expression = mFunctionName + "(";
            string split = "";
            for (auto& name : node->GetInputOrder()) {
                auto input = node->GetInput(name);
                if (input->GetConnector()) {
                    expression += split + input->GetConnector()->GetVariable(context->GetScope());
                } else {
                    expression += split + input->GetVariable(context->GetScope());
                }
                split = ", ";
            }
            for (auto& name : node->GetOutputOrder()) {
                auto output = node->GetOutput(name);
                expression += split + output->GetVariable(context->GetScope());
                stage->EmitVariableDeclaration(output, context, EMPTY_STRING, true);
            }
            expression += ");";

            stage->EmitLine(expression);
        }
    }
}

void SourceCodeShaderNode::CreateVariables(ShaderNodePtr node, ShaderContextPtr context,
                                           ShaderPtr shader) {
    auto& shaderGen = context->GetShaderGenerator();

    ShaderStagePtr vs = shader->GetStage(Stage::VERTEX);
    ShaderStagePtr ps = shader->GetStage(Stage::PIXEL);
    shaderGen.AddConnectorVariable("vertexData", "float3", ShaderBuildInVariable::POSITION_WORLD,
                                   vs, ps);
}

}  // namespace Ethereal
