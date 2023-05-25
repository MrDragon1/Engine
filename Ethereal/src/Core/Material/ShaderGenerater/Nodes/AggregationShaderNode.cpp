#include "AggregationShaderNode.h"
#include "Core/Material/ShaderGenerater/ShaderGraph.h"

namespace Ethereal {
AggregationShaderNode::AggregationShaderNode(NodeGraphPtr graph) : ShaderNodeImpl(graph) {
    mGraph = graph->GetDocument()->GenerateShaderGraphFromNodeGraph(graph);
}

void AggregationShaderNode::EmitFunctionCall(ShaderNodePtr node) {
    ET_CORE_INFO("{");
    mGraph->EmitString();

    string expression = "";
    for (auto& [name, output] : node->GetOutputs()) {
        expression += output->GetVariable() + " = " +
                      output->GetConnectorSocket()->GetConnector()->GetVariable() + ";";
    }
    ET_CORE_INFO(expression);
    ET_CORE_INFO("}");
}

}  // namespace Ethereal
