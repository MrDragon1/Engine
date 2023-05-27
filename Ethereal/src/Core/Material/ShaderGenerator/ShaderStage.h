#pragma once
#include "Core/Material/ShaderGenerator/ShaderGraph.h"

namespace Ethereal {
using VariableBlockPtr = Ref<class VariableBlock>;
using VariableBlockMap = unordered_map<string, VariableBlockPtr>;
using ShaderStagePtr = Ref<class ShaderStage>;

class VariableBlock : public RefCounted {
   public:
    VariableBlock(const string& name, const string& instance) : mName(name), mInstance(instance) {}

    const string& GetName() { return mName; }

    const string& GetInstance() { return mInstance; }

    const vector<ShaderPortPtr> GetVariables() const { return mVariablesOrder; }

    ShaderPortPtr operator[](const string& name) { return mVariables[name]; }

    ShaderPortPtr Add(ShaderNode* parent, const string& type, const string& name);

    void Add(ShaderPortPtr port);

    size_t size() const { return mVariables.size(); }

    bool empty() const { return mVariables.empty(); }

   public:
    string mName;
    string mInstance;
    unordered_map<string, ShaderPortPtr> mVariables;
    vector<ShaderPortPtr> mVariablesOrder;
};

class ShaderStage : public RefCounted {
   public:
    ShaderStage(const string& name) : mName(name) {}

    const string& GetName() { return mName; }

    void SetSourceCode(const string& code) { mCode = code; }

    const string& GetSourceCode() { return mCode; }

    VariableBlockPtr CreateUniformBlock(const string& name, const string& instance = EMPTY_STRING);

    VariableBlockPtr CreateInputBlock(const string& name, const string& instance = EMPTY_STRING);

    VariableBlockPtr CreateOutputBlock(const string& name, const string& instance = EMPTY_STRING);

    VariableBlock& GetUniformBlock(const string& name);

    VariableBlock& GetInputBlock(const string& name);

    VariableBlock& GetOutputBlock(const string& name);

    VariableBlockMap& GetUniformBlocks() { return mUniforms; }

    VariableBlockMap& GetInputBlocks() { return mInputs; }

    VariableBlockMap& GetOutputBlocks() { return mOutputs; }

    void AddFunctionDefinition(ShaderNodePtr node, ShaderContextPtr context);

    void AddFunctionCall(ShaderNodePtr node, ShaderContextPtr context);

    void EmitString(const string& str);

    void EmitComment(const string& str);

    void EmitLine(const string& str = EMPTY_STRING);

    void EmitVariableDeclaration(ShaderPortPtr data, ShaderContextPtr context,
                                 const string& prefix = EMPTY_STRING, bool initialvalue = false);

    void BeginLine();
    void EndLine();

    void BeginScope();
    void EndScope(const string& endwith = EMPTY_STRING);

   public:
    string mName;
    string mCode;
    VariableBlockMap mUniforms;
    VariableBlockMap mInputs;
    VariableBlockMap mOutputs;

   private:
    string mIndention = "";
};

}  // namespace Ethereal
