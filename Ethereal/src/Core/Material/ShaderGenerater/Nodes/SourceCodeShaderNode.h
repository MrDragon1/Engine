#pragma once
#include "Core/Material/ShaderGenerater/ShaderNode.h"

namespace Ethereal {
using SourceCodeShaderNodePtr = Ref<class SourceCodeShaderNode>;
class SourceCodeShaderNode : public ShaderNodeImpl {
   public:
    SourceCodeShaderNode(NodeImplPtr impl);
    void EmitFunctionCall(ShaderNodePtr node);

   private:
    bool mInlined;
    string mFunctionName;
    string mFunctionSource;
    string mSourceFilename;
};
}  // namespace Ethereal