#pragma once
#include "Core/Material/ShaderGenerator/ShaderGraph.h"
#include "Core/Material/ShaderGenerator/ShaderGenerator.h"

namespace Ethereal {
using ShaderContextPtr = Ref<class ShaderContext>;

class ShaderContext : public RefCounted {
   public:
    ShaderContext(){};
    ShaderGenerator& GetShaderGenerator() { return mShaderGenerator; }
    ShaderGraphPtr GetShaderGraph() { return mShaderGraph; }
    ShaderPtr GetShader() { return mShader; }
    string GetScope() { return mCurrentScope; }

    void PushScope(const string& scope) {
        if (!mCurrentScope.empty()) mScopeStack.push(mCurrentScope);
        mCurrentScope = scope;
    }

    void PopScope() {
        if (mCurrentScope.empty()) return;
        mCurrentScope = mScopeStack.top();
        mScopeStack.pop();
    }

   public:
    ShaderPtr mShader;
    ShaderGraphPtr mShaderGraph;
    ShaderGenerator mShaderGenerator;
    string mCurrentScope;
    stack<string> mScopeStack;
};
}  // namespace Ethereal
