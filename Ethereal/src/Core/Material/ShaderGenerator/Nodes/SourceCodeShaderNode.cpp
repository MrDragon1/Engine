#include "SourceCodeShaderNode.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
#include "Utils/StringUtils.h"
namespace Ethereal {
SourceCodeShaderNode::SourceCodeShaderNode() {}

ShaderNodeImplPtr SourceCodeShaderNode::Create() { return SourceCodeShaderNodePtr::Create(); }

void SourceCodeShaderNode::Initilize(ElementPtr elem, ShaderContextPtr context) {
    auto impl = elem.As<NodeImpl>();
    ShaderNodeImpl::Initilize(impl, context);
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
    }
    if (stage->GetName() == Stage::PIXEL) {
        if (mInlined) {
            string sourcecode = mFunctionSource;
            StringMap map;
            for (auto& name : node->GetInputOrder()) {
                auto input = node->GetInput(name);
                map[name] = GetInputVariable(input, context);
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
                expression += split + GetInputVariable(input, context);
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
                                           ShaderPtr shader) {}

}  // namespace Ethereal
