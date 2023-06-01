#pragma once
#include "Core/Material/ShaderGenerator/ShaderNode.h"

namespace Ethereal {
using ClosureAddShaderNodePtr = Ref<class ClosureAddShaderNode>;
class ClosureAddShaderNode : public ShaderNodeImpl {
   public:
    ClosureAddShaderNode();
    static ShaderNodeImplPtr Create();
    void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                          ShaderStagePtr stage) override;

    static const string IN1;
    static const string IN2;
};
}  // namespace Ethereal
