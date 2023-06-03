#pragma once
#include "Core/Material/ShaderGenerator/ShaderNode.h"

namespace Ethereal {
using SurfaceNodePtr = Ref<class SurfaceNode>;
class SurfaceNode : public ShaderNodeImpl {
   public:
    SurfaceNode();
    static ShaderNodeImplPtr Create();

    void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                          ShaderStagePtr stage) override;
    void CreateVariables(ShaderNodePtr node, ShaderContextPtr context, ShaderPtr shader) override;

    void EmitLightLoop(ShaderNodePtr node, ShaderContextPtr context, ShaderStagePtr stage,
                       const string outColor);

   private:
};
}  // namespace Ethereal