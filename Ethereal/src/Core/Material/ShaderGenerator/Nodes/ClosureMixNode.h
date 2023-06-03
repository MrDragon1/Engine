#pragma once

#include "Core/Material/ShaderGenerator/ShaderNode.h"

namespace Ethereal {
using ClosureMixNodePtr = Ref<class ClosureMixNode>;
class ClosureMixNode : public ShaderNodeImpl {
   public:
    ClosureMixNode();
    static ShaderNodeImplPtr Create();
    void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                          ShaderStagePtr stage) override;

    static const string FG;
    static const string BG;
    static const string MIX;
};
}  // namespace Ethereal
