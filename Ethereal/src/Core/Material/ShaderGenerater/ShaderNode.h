#pragma once
#include "Core/Material/MaterialGraph.h"

namespace Ethereal {
class ShaderContext;
using ShaderInputPtr = Ref<class ShaderInput>;
using ShaderOutputPtr = Ref<class ShaderOutput>;
using ShaderNodePtr = Ref<class ShaderNode>;
using ShaderNodeImplPtr = Ref<class ShaderNodeImpl>;
using ShaderInputSocketPtr = ShaderOutputPtr;
using ShaderOutputSocketPtr = ShaderInputPtr;
class ShaderInput : public RefCounted {
   public:
    ShaderInput(ShaderNodePtr parent, NodeInputPtr input, bool socket = false);
    ShaderInput(ShaderNodePtr parent, NodeOutputPtr inputSocket, bool socket = false);

    ElementPtr GetSource() { return mSource; }
    ShaderNodePtr GetParent() { return mParent; }
    string GetName() { return mName; }
    ShaderOutputPtr GetConnector() { return mConnector; }
    void SetConnector(ShaderOutputPtr conn);
    void Disconnect() { SetConnector(nullptr); }

    void SetVariable(const string& var) { mVariable = var; }
    string GetVariable() { return mVariable; }
    bool IsSocket() { return mIsSocket; }

   private:
    bool mIsSocket;
    string mName;
    string mVariable;
    ElementPtr mSource;
    ShaderNodePtr mParent;
    ShaderOutputPtr mConnector;
};
class ShaderOutput : public RefCounted {
   public:
    ShaderOutput(ShaderNodePtr parent, NodeOutputPtr output, bool socket = false);
    ShaderOutput(ShaderNodePtr parent, NodeInputPtr outputSocket, bool socket = false);

    ElementPtr GetSource() { return mSource; }

    ShaderNodePtr GetParent() { return mParent; }
    string GetName() { return mName; }
    vector<ShaderInputPtr> GetConnectors() { return mConnectors; }
    ShaderOutputSocketPtr GetConnectorSocket();

    void AddConnector(ShaderInputPtr conn);
    void RemoveConnector(ShaderInputPtr conn);

    void SetVariable(const string& var) { mVariable = var; }
    string GetVariable() { return mVariable; }
    bool IsSocket() { return mIsSocket; }

   private:
    bool mIsSocket;
    string mName;
    string mVariable;
    ElementPtr mSource;
    ShaderNodePtr mParent;
    vector<ShaderInputPtr> mConnectors;
};

class ShaderNodeImpl : public RefCounted {
   public:
    ShaderNodeImpl(ElementPtr elem) : mSource(elem){};
    virtual void EmitFunctionCall(ShaderNodePtr node){};
    virtual void EmitFunctionDefinition(ShaderNodePtr node){};
    virtual void CreateVariable(){};

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

    void Emit() { mImpl->EmitFunctionCall(this); }

    void Initalize();
    string GetName() { return mName; }

   private:
    string mName;
    unordered_map<string, ShaderInputPtr> mInputs;
    unordered_map<string, ShaderOutputPtr> mOutputs;
    ShaderNodeImplPtr mImpl;

    ElementPtr mSource;
    ShaderGraphPtr mGraph;
};
}  // namespace Ethereal
