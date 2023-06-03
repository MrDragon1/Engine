#pragma once

#include "Core/Material/ShaderGenerator/ShaderNode.h"

namespace Ethereal {
using BitangentNodePtr = Ref<class BitangentNode>;
class BitangentNode : public ShaderNodeImpl {
   public:
    BitangentNode();
    static ShaderNodeImplPtr Create();

    void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                          ShaderStagePtr stage) override;
    void CreateVariables(ShaderNodePtr node, ShaderContextPtr context, ShaderPtr shader) override;

   private:
};
}  // namespace Ethereal
