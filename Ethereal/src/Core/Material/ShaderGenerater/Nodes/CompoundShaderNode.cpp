#include "CompoundShaderNode.h"

namespace Ethereal {
CompoundShaderNode::CompoundShaderNode(NodeImplPtr impl) : ShaderNodeImpl(impl) {
    NodeGraphPtr graph = impl->GetNodeGraph();

    mGraph = graph->GetDocument()->GenerateShaderGraphFromNodeGraph(graph);
}

void CompoundShaderNode::EmitFunctionCall(ShaderNodePtr node) {
    std::string expression;
    expression += node->GetName() + "(";

    string split = "";
    for (auto& name : node->GetInputOrder()) {
        auto input = node->GetInput(name);
        if (input->GetConnector()) {
            expression += split + input->GetConnector()->GetVariable();
        } else {
            expression += split + input->GetVariable();
        }
        split = ", ";
    }
    for (auto& name : node->GetOutputOrder()) {
        auto output = node->GetOutput(name);
        expression += split + output->GetVariable();
    }
    expression += ");";

    ET_CORE_INFO(expression);
}

}  // namespace Ethereal
