#pragma once
#include "Core/Material/ShaderGenerator/ShaderNode.h"

namespace Ethereal {
using PositionNodePtr = Ref<class PositionNode>;
class PositionNode : public ShaderNodeImpl {
   public:
    PositionNode();
    static ShaderNodeImplPtr Create();

    void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                          ShaderStagePtr stage) override;
    void CreateVariables(ShaderNodePtr node, ShaderContextPtr context, ShaderPtr shader) override;

   private:
};
}  // namespace Ethereal