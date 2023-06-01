#pragma once

#include "Core/Material/ShaderGenerator/ShaderNode.h"

namespace Ethereal {
using ClosureMixShaderNodePtr = Ref<class ClosureMixShaderNode>;
class ClosureMixShaderNode : public ShaderNodeImpl {
   public:
    ClosureMixShaderNode();
    static ShaderNodeImplPtr Create();
    void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                          ShaderStagePtr stage) override;

    static const string FG;
    static const string BG;
    static const string MIX;
};
}  // namespace Ethereal
