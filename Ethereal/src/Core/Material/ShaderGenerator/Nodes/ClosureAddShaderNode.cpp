#include "ClosureAddShaderNode.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {
const string ClosureAddShaderNode::IN1 = "in1";
const string ClosureAddShaderNode::IN2 = "in2";

ClosureAddShaderNode::ClosureAddShaderNode() {}

ShaderNodeImplPtr ClosureAddShaderNode::Create() { return ClosureAddShaderNodePtr::Create(); }

void ClosureAddShaderNode::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                            ShaderStagePtr stage) {
    if (stage->GetName() == Stage::PIXEL) {
        auto& shaderGen = context->GetShaderGenerator();
        auto in1 = node->GetInput(IN1);
        auto in2 = node->GetInput(IN2);
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
