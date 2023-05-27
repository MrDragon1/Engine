#include "CompoundShaderNode.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {
CompoundShaderNode::CompoundShaderNode(NodeImplPtr impl) : ShaderNodeImpl(impl) {
    NodeGraphPtr graph = impl->GetNodeGraph();

    mGraph = graph->GetDocument()->GenerateShaderGraphFromNodeGraph(graph);
}

void CompoundShaderNode::EmitFunctionDefinition(ShaderNodePtr node, ShaderContextPtr context,
                                                ShaderStagePtr stage) {
    if (stage->GetName() == Stage::VERTEX) {
    }
    if (stage->GetName() == Stage::PIXEL) {
        // Emit function define that this function will use
        for (auto& snode : mGraph->GetSortedNodes()) {
            stage->AddFunctionDefinition(snode, context);
        }

        // Emit function call
        context->PushScope(mGraph->GetName());
        string expression = "void " + mGraph->GetName() + "(";
        string split = "";
        for (auto& [name, input] : mGraph->GetInputSockets()) {
            expression += split + input->GetValue()->GetSyntaxString() + " " +
                          input->GetVariable(context->GetScope());
            split = ", ";
        }
        for (auto& [name, output] : mGraph->GetOutputSockets()) {
            expression += split + "out " + output->GetValue()->GetSyntaxString() + " " +
                          output->GetVariable(context->GetScope());
        }
        expression += ")";

        stage->EmitLine(expression);

        stage->BeginScope();
        for (auto& snode : mGraph->GetSortedNodes()) {
            stage->AddFunctionCall(snode, context);
        }

        for (auto& [name, output] : mGraph->GetOutputSockets()) {
            stage->EmitLine(output->GetVariable(context->GetScope()) + " = " +
                            output->GetConnector()->GetVariable(context->GetScope()) + ";");
        }
        stage->EndScope();

        stage->EmitLine("");

        context->PopScope();
    }
}

void CompoundShaderNode::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
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
                    mGraph->GetInputSocket(input->GetName())->GetVariable(context->GetScope());
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

void CompoundShaderNode::CreateVariables(ShaderNodePtr node, ShaderContextPtr context,
                                         ShaderPtr shader) {
    auto& uniformBlock = context->GetShader()
                             ->GetStage(Stage::PIXEL)
                             ->GetUniformBlock(ShaderBuildInVariable::PUBUNIFORM);
    for (auto& name : node->GetInputOrder()) {
        auto input = node->GetInput(name);
        if (!input->GetConnector()) {
            uniformBlock.Add(mGraph->GetInputSocket(input->GetName()));
        }
    }
}

}  // namespace Ethereal