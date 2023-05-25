#include "SourceCodeShaderNode.h"
#include "Core/Material/ShaderGenerater/ShaderGraph.h"

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

void SourceCodeShaderNode::EmitFunctionCall(ShaderNodePtr node) {
    if (mInlined) {
        string sourcecode = mFunctionSource;
        StringMap map;
        for (auto& [name, input] : node->GetInputs()) {
            auto output = input->GetConnector();
            if (output) {
                map[name] = output->GetVariable();
            } else {
                map[name] = "Unknown value";
            }
        }
        Utils::ReplaceIdentifier(sourcecode, map);

        string expression = node->GetOutput()->GetVariable() + " = " + sourcecode + ";";

        ET_CORE_INFO(expression);
    } else {
        string expression = mFunctionName + "(";
        string split = "";
        for (auto& [name, input] : node->GetInputs()) {
            if (input->GetConnector()) {
                expression += split + input->GetConnector()->GetVariable();
            } else {
                expression += split + input->GetVariable();
            }
            split = ", ";
        }
        for (auto& [name, output] : node->GetOutputs()) {
            expression += split + output->GetVariable();
        }
        expression += ");";

        ET_CORE_INFO(expression);
    }
}

}  // namespace Ethereal
