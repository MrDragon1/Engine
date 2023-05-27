#pragma once
#include "Core/Material/MaterialGraph.h"

namespace Ethereal {
using ShaderContextPtr = Ref<class ShaderContext>;
using ShaderStagePtr = Ref<class ShaderStage>;
using ShaderPtr = Ref<class Shader>;

using ShaderPortPtr = Ref<class ShaderPort>;
using ShaderInputPtr = Ref<class ShaderInput>;
using ShaderOutputPtr = Ref<class ShaderOutput>;
using ShaderNodePtr = Ref<class ShaderNode>;
using ShaderNodeImplPtr = Ref<class ShaderNodeImpl>;
using ShaderInputSocketPtr = ShaderOutputPtr;
using ShaderOutputSocketPtr = ShaderInputPtr;

class ShaderPortFlag {
   public:
    static const uint32_t UNIFORM = 1u << 0;
    static const uint32_t EMITTED = 1u << 1;
    static const uint32_t BIND_INPUT = 1u << 2;
};

class ShaderPort : public RefCounted {
   public:
    ShaderPort(ShaderNodePtr parent, ElementPtr source, bool socket = false);
    ShaderPort(ShaderNodePtr parent, const string& type, const string& name);

    ElementPtr GetSource() { return mSource; }
    ShaderNodePtr GetParent() { return mParent; }
    string GetName() { return mName; }
    void SetValue(ValueBasePtr value) { mValue = value; }
    ValueBasePtr GetValue() { return mValue; }
    void SetVariable(const string& var) { mVariable = var; }
    string GetVariable(const string& scope) {
        string var = mVariable;
        if (!scope.empty() && var.substr(0, scope.size()) == scope)
            return mVariable.substr(scope.size() + 1);
        else
            return mVariable;
    }
    bool IsSocket() { return mIsSocket; }

    void SetEmitted() { mFlags |= ShaderPortFlag::EMITTED; }
    bool IsEmitted() const { return (mFlags & ShaderPortFlag::EMITTED) != 0; }

   private:
    bool mIsSocket;
    string mName;
    string mVariable;
    ValueBasePtr mValue;
    ElementPtr mSource;
    ShaderNodePtr mParent;
    uint32_t mFlags;
};

class ShaderInput : public ShaderPort {
   public:
    ShaderInput(ShaderNodePtr parent, NodeInputPtr input, bool socket = false);
    ShaderInput(ShaderNodePtr parent, NodeOutputPtr inputSocket, bool socket = false);

    ShaderOutputPtr GetConnector() { return mConnector; }
    void SetConnector(ShaderOutputPtr conn);
    void Disconnect() { SetConnector(nullptr); }

   private:
    ShaderOutputPtr mConnector;
};
class ShaderOutput : public ShaderPort {
   public:
    ShaderOutput(ShaderNodePtr parent, NodeOutputPtr output, bool socket = false);
    ShaderOutput(ShaderNodePtr parent, NodeInputPtr outputSocket, bool socket = false);

    vector<ShaderInputPtr> GetConnectors() { return mConnectors; }
    ShaderOutputSocketPtr GetConnectorSocket();

    void AddConnector(ShaderInputPtr conn);
    void RemoveConnector(ShaderInputPtr conn);

   private:
    vector<ShaderInputPtr> mConnectors;
};

class ShaderNodeImpl : public RefCounted {
   public:
    ShaderNodeImpl(ElementPtr elem) : mSource(elem){};
    virtual void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                  ShaderStagePtr stage){};
    virtual void EmitFunctionDefinition(ShaderNodePtr node, ShaderContextPtr context,
                                        ShaderStagePtr stage){};
    virtual void CreateVariables(ShaderNodePtr node, ShaderContextPtr context, ShaderPtr shader){};

   private:
    ElementPtr mSource;
};

class ShaderNode : public RefCounted {
   public:
    ShaderNode(ElementPtr node, ShaderGraphPtr graph);
    void AddInput(NodeInputPtr input);
    void AddOutput(NodeOutputPtr output);

    ShaderInputPtr GetInput(const string& name);
    ShaderOutputPtr GetOutput(const string& name = EMPTY_STRING);

    unordered_map<string, ShaderInputPtr> GetInputs() { return mInputs; }
    unordered_map<string, ShaderOutputPtr> GetOutputs() { return mOutputs; }

    vector<string> GetInputOrder() { return mInputOrder; }
    vector<string> GetOutputOrder() { return mOutputOrder; }

    void Initalize();
    string GetName() { return mName; }
    ShaderNodeImplPtr GetImpl() { return mImpl; }

    void EmitFunctionDefinition(ShaderContextPtr context, ShaderStagePtr stage) {
        mImpl->EmitFunctionDefinition(this, context, stage);
    }
    void EmitFunctionCall(ShaderContextPtr context, ShaderStagePtr stage) {
        mImpl->EmitFunctionCall(this, context, stage);
    }

   private:
    string mName;
    unordered_map<string, ShaderInputPtr> mInputs;
    unordered_map<string, ShaderOutputPtr> mOutputs;
    ShaderNodeImplPtr mImpl;

    vector<string> mOutputOrder;
    vector<string> mInputOrder;

    ElementPtr mSource;
    ShaderGraphPtr mGraph;
};
}  // namespace Ethereal
