#pragma once
#include "Core/Material/ShaderGenerater/ShaderNode.h"
#include "Core/Material/ShaderGenerater/ShaderGraph.h"

namespace Ethereal {
using CompoundShaderNodePtr = Ref<class CompoundShaderNode>;
class CompoundShaderNode : public ShaderNodeImpl {
   public:
    CompoundShaderNode(NodeImplPtr impl);
    void EmitFunctionCall(ShaderNodePtr node);
    ShaderGraphPtr GetGraph() { return mGraph; }

   private:
    ShaderGraphPtr mGraph;
};
}  // namespace Ethereal
