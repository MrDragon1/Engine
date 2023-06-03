#include "ClosureAddNode.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {
const string ClosureAddNode::IN1 = "in1";
const string ClosureAddNode::IN2 = "in2";

ClosureAddNode::ClosureAddNode() {}

ShaderNodeImplPtr ClosureAddNode::Create() { return ClosureAddNodePtr::Create(); }

void ClosureAddNode::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                      ShaderStagePtr stage) {
    if (stage->GetName() == Stage::PIXEL) {
        auto& shaderGen = context->GetShaderGenerator();
        auto in1 = node->GetInput(IN1);
        auto in2 = node->GetInput(IN2);

        if (in1->GetConnector()) {
            ShaderNodePtr in1Node = in1->GetConnector()->GetParent();
            if (in1Node->GetGraph() == node->GetGraph()) {
                shaderGen.EmitFunctionCall(in1Node, context, stage);
            }
        }
        if (in2->GetConnector()) {
            ShaderNodePtr in2Node = in2->GetConnector()->GetParent();
            if (in2Node->GetGraph() == node->GetGraph()) {
                shaderGen.EmitFunctionCall(in2Node, context, stage);
            }
        }

        auto output = node->GetOutput();
        if (output->GetValue()->Is<BSDF>()) {
            stage->EmitVariableDeclaration(output, context, "", true);
            stage->EmitLine(output->GetVariable(context->GetScope()) +
                            ".response = " + GetInputVariable(in1, context) + ".response + " +
                            GetInputVariable(in2, context) + ".response;");
            stage->EmitLine(output->GetVariable(context->GetScope()) +
                            ".throughput = " + GetInputVariable(in1, context) + ".throughput * " +
                            GetInputVariable(in2, context) + ".throughput;");
        }
    }
}

}  // namespace Ethereal
