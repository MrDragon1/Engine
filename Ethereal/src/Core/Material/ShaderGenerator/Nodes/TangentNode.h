#pragma once

#include "Core/Material/ShaderGenerator/ShaderNode.h"

namespace Ethereal {
using TangentNodePtr = Ref<class TangentNode>;
class TangentNode : public ShaderNodeImpl {
   public:
    TangentNode();
    static ShaderNodeImplPtr Create();

    void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                          ShaderStagePtr stage) override;
    void CreateVariables(ShaderNodePtr node, ShaderContextPtr context, ShaderPtr shader) override;

   private:
};
}  // namespace Ethereal
