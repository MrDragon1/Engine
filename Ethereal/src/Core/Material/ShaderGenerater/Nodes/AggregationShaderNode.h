#pragma once
#include "Core/Material/ShaderGenerater/ShaderNode.h"

namespace Ethereal {
using AggregationShaderNodePtr = Ref<class AggregationShaderNode>;
class AggregationShaderNode : public ShaderNodeImpl {
   public:
    AggregationShaderNode(NodeGraphPtr graph);
    void EmitFunctionCall(ShaderNodePtr node);
    ShaderGraphPtr GetGraph() { return mGraph; }

   private:
    ShaderGraphPtr mGraph;
};
}  // namespace Ethereal
