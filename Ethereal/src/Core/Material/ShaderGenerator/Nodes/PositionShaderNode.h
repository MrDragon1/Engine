#pragma once
#include "Core/Material/ShaderGenerator/ShaderNode.h"

namespace Ethereal {
using PositionShaderNodePtr = Ref<class PositionShaderNode>;
class PositionShaderNode : public ShaderNodeImpl {
   public:
    PositionShaderNode();
    static ShaderNodeImplPtr Create();

    void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                          ShaderStagePtr stage) override;
    void CreateVariables(ShaderNodePtr node, ShaderContextPtr context, ShaderPtr shader) override;

   private:
};
}  // namespace Ethereal