#pragma once
#include "Core/Material/ShaderGenerator/ShaderContext.h"

namespace Ethereal {
// TODO: Rename this class to Material and remove the previous Material System
using MaterialCorePtr = Ref<class MaterialCore>;
class MaterialCore : public Asset {
   public:
    MaterialCore() = default;

    void SetDocument(DocumentPtr doc) { mDocument = doc; }
    DocumentPtr GetDocument() { return mDocument; }

    void Compile();

    ProgramHandle GetProgram() {
        if (mIsCompile)
            return mShaderContext->GetShader()->GetProgram();
        else
            return nullptr;
    }
    MaterialGraphPtr GenerateUIGraph() { return mDocument->GenerateUIGraph(); }

   private:
    bool mIsCompile = false;
    DocumentPtr mDocument;
    ShaderContextPtr mShaderContext;
};
}  // namespace Ethereal
