#pragma once
#include "Core/Material/ShaderGenerator/ShaderNode.h"

namespace Ethereal {
using ClosureAddNodePtr = Ref<class ClosureAddNode>;
class ClosureAddNode : public ShaderNodeImpl {
   public:
    ClosureAddNode();
    static ShaderNodeImplPtr Create();
    void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                          ShaderStagePtr stage) override;

    static const string IN1;
    static const string IN2;
};
}  // namespace Ethereal
