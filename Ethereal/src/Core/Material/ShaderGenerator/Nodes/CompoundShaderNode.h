#pragma once
#include "Core/Material/ShaderGenerator/ShaderNode.h"
#include "Core/Material/ShaderGenerator/ShaderGraph.h"

namespace Ethereal {
using CompoundShaderNodePtr = Ref<class CompoundShaderNode>;
class CompoundShaderNode : public ShaderNodeImpl {
   public:
    CompoundShaderNode();
    static ShaderNodeImplPtr Create();
    void Initilize(ElementPtr elem, ShaderContextPtr context) override;
    void EmitFunctionDefinition(ShaderNodePtr node, ShaderContextPtr context,
                                ShaderStagePtr stage) override;
    void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                          ShaderStagePtr stage) override;
    void CreateVariables(ShaderNodePtr node, ShaderContextPtr context, ShaderPtr shader) override;
    ShaderGraphPtr GetGraph() { return mGraph; }

   private:
    ShaderGraphPtr mGraph;
};
}  // namespace Ethereal
