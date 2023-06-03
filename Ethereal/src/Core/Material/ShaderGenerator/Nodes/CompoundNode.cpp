#include "CompoundNode.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {
CompoundNode::CompoundNode() {}

ShaderNodeImplPtr CompoundNode::Create() { return CompoundNodePtr::Create(); }

void CompoundNode::Initilize(ElementPtr elem, ShaderContextPtr context) {
    auto impl = elem.As<NodeImpl>();
    ShaderNodeImpl::Initilize(impl, context);
    NodeGraphPtr graph = impl->GetNodeGraph();

    mGraph = graph->GetDocument()->GenerateShaderGraphFromNodeGraph(graph, context);
}

void CompoundNode::EmitFunctionDefinition(ShaderNodePtr node, ShaderContextPtr context,
                                          ShaderStagePtr stage) {
    if (stage->GetName() == Stage::VERTEX) {
    }
    if (stage->GetName() == Stage::PIXEL) {
        // Emit function define that this function will use
        context->GetShaderGenerator().EmitFunctionDefinitions(mGraph, context, stage);

        // Emit function call
        context->PushScope(mGraph->GetName());
        string expression = "void " + mGraph->GetName() + "(";
        string split = "";
        for (auto& sinput : node->GetInputOrder()) {
            auto input = mGraph->GetInputSocket(sinput);
            expression += split + input->GetValue()->GetSyntaxString() + " " +
                          input->GetVariable(context->GetScope());
            split = ", ";
        }
        for (auto& soutput : node->GetOutputOrder()) {
            auto output = mGraph->GetOutputSocket(soutput);
            expression += split + "out " + output->GetValue()->GetSyntaxString() + " " +
                          output->GetVariable(context->GetScope());
        }
        expression += ")";

        stage->EmitLine(expression);

        stage->BeginScope();
        context->GetShaderGenerator().EmitFunctionCalls(mGraph, context, stage);

        for (auto& [name, output] : mGraph->GetOutputSockets()) {
            stage->EmitLine(output->GetVariable(context->GetScope()) + " = " +
                            output->GetConnector()->GetVariable(context->GetScope()) + ";");
        }
        stage->EndScope();

        stage->EmitLine("");

        context->PopScope();
    }
}

void CompoundNode::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                    ShaderStagePtr stage) {
    if (stage->GetName() == Stage::VERTEX) {
    }
    if (stage->GetName() == Stage::PIXEL) {
        std::string expression;
        expression += mGraph->GetName() + "(";

        string split = "";
        for (auto& name : node->GetInputOrder()) {
            auto input = node->GetInput(name);
            if (input->GetConnector()) {
                expression += split + input->GetConnector()->GetVariable(context->GetScope());
            } else {
                expression +=
                    split +
                    mGraph->GetInputSocket(input->GetFullName())->GetVariable(context->GetScope());
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

void CompoundNode::CreateVariables(ShaderNodePtr node, ShaderContextPtr context, ShaderPtr shader) {
    auto& uniformBlock = context->GetShader()
                             ->GetStage(Stage::PIXEL)
                             ->GetUniformBlock(ShaderBuildInVariable::PSPUBUNIFORM);
    for (auto& snode : mGraph->GetSortedNodes()) {
        snode->GetImpl()->CreateVariables(node, context, shader);
    }
    for (auto& name : node->GetInputOrder()) {
        auto input = node->GetInput(name);
        if (!input->GetConnector()) {
            uniformBlock.Add(mGraph->GetInputSocket(input->GetFullName()));
        }
    }
}

}  // namespace Ethereal
