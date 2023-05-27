#pragma once
#include "Core/Material/ShaderGenerator/ShaderNode.h"

namespace Ethereal {
using AggregationShaderNodePtr = Ref<class AggregationShaderNode>;
class AggregationShaderNode : public ShaderNodeImpl {
   public:
    AggregationShaderNode(NodeGraphPtr graph);
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
