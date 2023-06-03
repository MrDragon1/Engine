#include "ClosureMixNode.h"

#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {
const string ClosureMixNode::FG = "fg";
const string ClosureMixNode::BG = "bg";
const string ClosureMixNode::MIX = "mix";

ClosureMixNode::ClosureMixNode() {}

ShaderNodeImplPtr ClosureMixNode::Create() { return ClosureMixNodePtr::Create(); }

void ClosureMixNode::EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                      ShaderStagePtr stage) {
    if (stage->GetName() == Stage::PIXEL) {
        auto& shaderGen = context->GetShaderGenerator();
        auto fg = node->GetInput(FG);
        auto bg = node->GetInput(BG);
        auto mix = node->GetInput(MIX);
        auto output = node->GetOutput();

        if (fg->GetConnector()) {
            ShaderNodePtr fgNode = fg->GetConnector()->GetParent();
            if (fgNode->GetGraph() == node->GetGraph()) {
                shaderGen.EmitFunctionCall(fgNode, context, stage);
            }
        }
        if (bg->GetConnector()) {
            ShaderNodePtr bgNode = bg->GetConnector()->GetParent();
            if (bgNode->GetGraph() == node->GetGraph()) {
                shaderGen.EmitFunctionCall(bgNode, context, stage);
            }
        }

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
