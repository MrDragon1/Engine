#pragma once
#include "Core/Material/ShaderGenerator/ShaderNode.h"

namespace Ethereal {
using ClosureMultiplyNodePtr = Ref<class ClosureMultiplyNode>;
class ClosureMultiplyNode : public ShaderNodeImpl {
   public:
    ClosureMultiplyNode();
    static ShaderNodeImplPtr Create();
    void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                          ShaderStagePtr stage) override;

    static const string IN1;
    static const string IN2;
};
}  // namespace Ethereal
