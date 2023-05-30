#include "AggregationShaderNode.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {
AggregationShaderNode::AggregationShaderNode() {}

ShaderNodeImplPtr AggregationShaderNode::Create() { return AggregationShaderNodePtr::Create(); }

void AggregationShaderNode::Initilize(ElementPtr elem, ShaderContextPtr context) {
    auto graph = elem.As<NodeGraph>();
    ShaderNodeImpl::Initilize(graph, context);
    mGraph = graph->GetDocument()->GenerateShaderGraphFromNodeGraph(graph, context);
}

void AggregationShaderNode::EmitFunctionDefinition(ShaderNodePtr node, ShaderContextPtr context,
                                                   ShaderStagePtr stage) {
    if (stage->GetName() == Stage::VERTEX) {
    }
    if (stage->GetName() == Stage::PIXEL) {
        for (auto& node : mGraph->GetSortedNodes()) {
            stage->AddFunctionDefinition(node, context);
        }
    }
}

void AggregationShaderNode::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                             ShaderStagePtr stage) {
    auto& shaderGen = context->GetShaderGenerator();
    if (stage->GetName() == Stage::VERTEX) {
    }
    if (stage->GetName() == Stage::PIXEL) {
        // stage->BeginScope();
        for (auto& snode : mGraph->GetSortedNodes()) {
            stage->AddFunctionCall(snode, context);
        }

        string expression = "";
        for (auto& name : node->GetOutputOrder()) {
            auto output = node->GetOutput(name);
            expression +=
                output->GetVariable(context->GetScope()) + " = " +
                mGraph->GetOutputSocket(name)->GetConnector()->GetVariable(context->GetScope()) +
                ";";
            stage->EmitVariableDeclaration(output, context, EMPTY_STRING, true);
        }
        stage->EmitLine(expression);
        // stage->EndScope();
        stage->EmitLine("");
    }
}

void AggregationShaderNode::CreateVariables(ShaderNodePtr node, ShaderContextPtr context,
                                            ShaderPtr shader) {
    auto& uniformBlock = context->GetShader()
                             ->GetStage(Stage::PIXEL)
                             ->GetUniformBlock(ShaderBuildInVariable::PSPUBUNIFORM);
    for (auto& snode : mGraph->GetSortedNodes()) {
        snode->GetImpl()->CreateVariables(node, context, shader);
    }
    for (auto& [name, socket] : mGraph->GetInputSockets()) {
        uniformBlock.Add(socket);
    }
}

}  // namespace Ethereal
