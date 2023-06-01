#include "ClosureMultiplyShaderNode.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {
const string ClosureMultiplyShaderNode::IN1 = "in1";
const string ClosureMultiplyShaderNode::IN2 = "in2";

ClosureMultiplyShaderNode::ClosureMultiplyShaderNode() {}

ShaderNodeImplPtr ClosureMultiplyShaderNode::Create() {
    return ClosureMultiplyShaderNodePtr::Create();
}

void ClosureMultiplyShaderNode::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                                 ShaderStagePtr stage) {
    if (stage->GetName() == Stage::PIXEL) {
        auto& shaderGen = context->GetShaderGenerator();
        auto in1 = node->GetInput(IN1);
        auto in2 = node->GetInput(IN2);
        auto output = node->GetOutput();
        if (output->GetValue()->Is<BSDF>()) {
            const string in2clamped = output->GetVariable(context->GetScope()) + "_in2_clamped";

            stage->EmitLine(in2->GetValue()->GetSyntaxString() + " " + in2clamped + " = clamp(" +
                            GetInputVariable(in2, context) + ", 0.0, 1.0);");

            stage->EmitVariableDeclaration(output, context, "", true);
            stage->EmitLine(output->GetVariable(context->GetScope()) + ".response = " +
                            GetInputVariable(in1, context) + ".response * " + in2clamped + ";");
            stage->EmitLine(output->GetVariable(context->GetScope()) + ".throughput = " +
                            GetInputVariable(in1, context) + ".throughput * " + in2clamped + ";");
        }
    }
}

}  // namespace Ethereal
