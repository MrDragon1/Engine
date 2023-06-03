#pragma once
#include "Core/Material/ShaderGenerator/ShaderNode.h"

namespace Ethereal {
using SourceCodeShaderNodePtr = Ref<class SourceCodeShaderNode>;
class SourceCodeShaderNode : public ShaderNodeImpl {
   public:
    SourceCodeShaderNode();
    static ShaderNodeImplPtr Create();
    void Initilize(ElementPtr elem, ShaderContextPtr context) override;
    void EmitFunctionDefinition(ShaderNodePtr node, ShaderContextPtr context,
                                ShaderStagePtr stage) override;
    void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                          ShaderStagePtr stage) override;
    void CreateVariables(ShaderNodePtr node, ShaderContextPtr context, ShaderPtr shader) override;

   private:
    bool mInlined;
    string mFunctionName;
    string mFunctionSource;
    string mSourceFilename;
};
}  // namespace Ethereal