#include "ClosureMixShaderNode.h"

#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {
const string ClosureMixShaderNode::FG = "fg";
const string ClosureMixShaderNode::BG = "bg";
const string ClosureMixShaderNode::MIX = "mix";

ClosureMixShaderNode::ClosureMixShaderNode() {}

ShaderNodeImplPtr ClosureMixShaderNode::Create() { return ClosureMixShaderNodePtr::Create(); }

void ClosureMixShaderNode::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                            ShaderStagePtr stage) {
    if (stage->GetName() == Stage::PIXEL) {
        auto& shaderGen = context->GetShaderGenerator();
        auto fg = node->GetInput(FG);
        auto bg = node->GetInput(BG);
        auto mix = node->GetInput(MIX);
        auto output = node->GetOutput();
        if (output->GetValue()->Is<BSDF>()) {
            stage->EmitVariableDeclaration(output, context, "", true);
            stage->EmitLine(output->GetVariable(context->GetScope()) + ".response = mix(" +
                            GetInputVariable(fg, context) + ".response, " +
                            GetInputVariable(bg, context) + ".response, " +
                            GetInputVariable(mix, context) + ");");
            stage->EmitLine(output->GetVariable(context->GetScope()) + ".throughput = mix(" +
                            GetInputVariable(fg, context) + ".throughput, " +
                            GetInputVariable(bg, context) + ".throughput, " +
                            GetInputVariable(mix, context) + ");");
        }
    }
}

}  // namespace Ethereal
