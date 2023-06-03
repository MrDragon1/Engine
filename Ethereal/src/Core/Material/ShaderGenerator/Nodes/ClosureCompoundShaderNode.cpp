#include "ClosureCompoundShaderNode.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {
ClosureCompoundShaderNode::ClosureCompoundShaderNode() {}

ShaderNodeImplPtr ClosureCompoundShaderNode::Create() {
    return ClosureCompoundShaderNodePtr::Create();
}

void ClosureCompoundShaderNode::Initilize(ElementPtr elem, ShaderContextPtr context) {
    auto impl = elem.As<NodeImpl>();
    ShaderNodeImpl::Initilize(impl, context);
    NodeGraphPtr graph = impl->GetNodeGraph();

    mGraph = graph->GetDocument()->GenerateShaderGraphFromNodeGraph(graph, context);
}

void ClosureCompoundShaderNode::EmitFunctionDefinition(ShaderNodePtr node, ShaderContextPtr context,
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
        for (auto& sinput : node->GetInputOrder()) {
            auto input = mGraph->GetInputSocket(sinput);
            if (input->IsGeom()) continue;
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

        for (auto& sinput : node->GetInputOrder()) {
            auto input = mGraph->GetInputSocket(sinput);
            if (input->IsGeom()) {
                stage->EmitGeometryConnection(input, context);
            }
        }

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

void ClosureCompoundShaderNode::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                                 ShaderStagePtr stage) {
    if (stage->GetName() == Stage::VERTEX) {
    }
    if (stage->GetName() == Stage::PIXEL) {
        std::string expression;
        expression += mGraph->GetName() + "(";

        string split = "";
        for (auto& name : node->GetInputOrder()) {
            auto input = node->GetInput(name);
            if (mGraph->GetInputSocket(name)->IsGeom()) continue;
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

void ClosureCompoundShaderNode::CreateVariables(ShaderNodePtr node, ShaderContextPtr context,
                                                ShaderPtr shader) {
    ShaderStagePtr vs = shader->GetStage(Stage::VERTEX);
    ShaderStagePtr ps = shader->GetStage(Stage::PIXEL);

    auto& uniformBlock = ps->GetUniformBlock(ShaderBuildInVariable::PSPUBUNIFORM);
    for (auto& snode : mGraph->GetSortedNodes()) {
        snode->GetImpl()->CreateVariables(node, context, shader);
    }
    for (auto& name : node->GetInputOrder()) {
        if (mGraph->GetInputSocket(name)->IsGeom()) continue;
        auto input = node->GetInput(name);
        if (!input->GetConnector()) {
            uniformBlock.Add(mGraph->GetInputSocket(input->GetFullName()));
        }
    }

    auto& prvUniformBlock = ps->GetUniformBlock(ShaderBuildInVariable::PRVUNIFORM);
    prvUniformBlock.Add(nullptr, "float3", ShaderBuildInVariable::VIEW_POSITION);
    prvUniformBlock.Add(nullptr, "float3", ShaderBuildInVariable::LIGHT_POSITION);
}

}  // namespace Ethereal
