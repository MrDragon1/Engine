#pragma once

#include "Core/Material/ShaderGenerator/ShaderNode.h"

namespace Ethereal {
using NormalNodePtr = Ref<class NormalNode>;
class NormalNode : public ShaderNodeImpl {
   public:
    NormalNode();
    static ShaderNodeImplPtr Create();

    void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                          ShaderStagePtr stage) override;
    void CreateVariables(ShaderNodePtr node, ShaderContextPtr context, ShaderPtr shader) override;

   private:
};
}  // namespace Ethereal
