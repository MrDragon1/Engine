#pragma once

#include "Core/Material/ShaderGenerator/ShaderNode.h"

namespace Ethereal {
using NormalShaderNodePtr = Ref<class NormalShaderNode>;
class NormalShaderNode : public ShaderNodeImpl {
   public:
    NormalShaderNode();
    static ShaderNodeImplPtr Create();

    void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                          ShaderStagePtr stage) override;
    void CreateVariables(ShaderNodePtr node, ShaderContextPtr context, ShaderPtr shader) override;

   private:
};
}  // namespace Ethereal
