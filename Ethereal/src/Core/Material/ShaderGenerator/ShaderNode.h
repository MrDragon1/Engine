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
    static const uint32_t GEOM = 1u << 2;
};

class ShaderPort : public RefCounted {
   public:
    ShaderPort(ShaderNodePtr parent, ElementPtr source, bool socket = false);
    ShaderPort(ShaderNodePtr parent, const string& type, const string& name);

    ElementPtr GetSource() { return mSource; }
    ShaderNodePtr GetParent() { return mParent; }
    string GetName() { return mName; }
    string GetFullName();
    void SetValue(ValueBasePtr value);
    ValueBasePtr GetValue() { return mValue; }
    void SetVariable(const string& var) { mVariable = var; }
    string GetVariable(const string& scope = EMPTY_STRING) {
        string var = mVariable;
        auto startpos = var.find_last_of('.');
        if (startpos == std::string::npos) startpos = -1;  // skip the '.' indexing
        if (!scope.empty() && var.substr(startpos + 1, scope.size()) == scope)
            return mVariable.substr(startpos + 1 + scope.size() + 1) + mChannels;
        else
            return mVariable + mChannels;
    }
    string GetChannels() { return mChannels; }

    bool IsSocket() { return mIsSocket; }

    void SetEmitted() { mFlags |= ShaderPortFlag::EMITTED; }
    bool IsEmitted() const { return (mFlags & ShaderPortFlag::EMITTED) != 0; }
    void SetUniform() { mFlags |= ShaderPortFlag::UNIFORM; }
    bool IsUniform() const { return (mFlags & ShaderPortFlag::UNIFORM) != 0; }
    void SetGeom() { mFlags |= ShaderPortFlag::GEOM; }
    bool IsGeom() const { return (mFlags & ShaderPortFlag::GEOM) != 0; }

   private:
    bool mIsSocket;
    string mName;
    string mVariable;
    ValueBasePtr mValue;
    ElementPtr mSource;
    ShaderNodePtr mParent;
    uint32_t mFlags = 0;
    string mChannels = "";
};

class ShaderInput : public ShaderPort {
   public:
    ShaderInput(ShaderNodePtr parent, NodeInputPtr input, bool socket = false);
    ShaderInput(ShaderNodePtr parent, NodeOutputPtr inputSocket, bool socket = false);

    ShaderOutputPtr GetConnector() { return mConnector; }
    void SetConnector(ShaderOutputPtr conn);

    ShaderNodePtr GetSibling();

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
    virtual void Initilize(ElementPtr elem, ShaderContextPtr context) { mSource = elem; }
    virtual void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context,
                                  ShaderStagePtr stage) {}
    virtual void EmitFunctionDefinition(ShaderNodePtr node, ShaderContextPtr context,
                                        ShaderStagePtr stage) {}
    virtual void CreateVariables(ShaderNodePtr node, ShaderContextPtr context, ShaderPtr shader) {}

    string GetInputVariable(ShaderInputPtr port, ShaderContextPtr context);

   protected:
    ShaderNodeImpl(){};

   private:
    ElementPtr mSource;
};

class ShaderNode : public RefCounted {
   public:
    class Classification {
       public:
        static const uint32_t DEFAULT =
            1 << 0;  /// Any node that outputs floats, colors, vectors, etc.
        static const uint32_t CLOSURE = 1 << 1;   /// Any node that represents light integration
        static const uint32_t SHADER = 1 << 2;    /// Any node that outputs a shader
        static const uint32_t MATERIAL = 1 << 3;  /// Any node that outputs a material

        // Specific closure types
        static const uint32_t BSDF = 1 << 7;    /// A BSDF node
        static const uint32_t BSDF_R = 1 << 8;  /// A reflection BSDF node
        static const uint32_t BSDF_T = 1 << 9;  /// A transmission BSDF node
        static const uint32_t EDF = 1 << 10;    /// A EDF node
        static const uint32_t VDF = 1 << 11;    /// A VDF node

        // Specific shader types
        static const uint32_t SURFACE = 1 << 14;  /// A surface shader node
        static const uint32_t VOLUME = 1 << 15;   /// A volume shader node
        static const uint32_t LIGHT = 1 << 16;    /// A light shader node
        static const uint32_t UNLIT = 1 << 17;    /// An unlit surface shader node
    };

    ShaderNode(ElementPtr node, ShaderGraphPtr graph, ShaderContextPtr context);
    void AddInput(NodeInputPtr input);
    void AddOutput(NodeOutputPtr output);

    ShaderInputPtr GetInput(const string& name);
    ShaderOutputPtr GetOutput(const string& name = EMPTY_STRING);

    unordered_map<string, ShaderInputPtr> GetInputs() { return mInputs; }
    unordered_map<string, ShaderOutputPtr> GetOutputs() { return mOutputs; }

    vector<string> GetInputOrder() { return mInputOrder; }
    vector<string> GetOutputOrder() { return mOutputOrder; }

    void Initalize(ShaderContextPtr context);
    string GetName() { return mName; }
    ShaderGraphPtr GetGraph() { return mGraph; }
    ShaderNodeImplPtr GetImpl() { return mImpl; }

    void EmitFunctionDefinition(ShaderContextPtr context, ShaderStagePtr stage) {
        mImpl->EmitFunctionDefinition(this, context, stage);
    }
    void EmitFunctionCall(ShaderContextPtr context, ShaderStagePtr stage) {
        mImpl->EmitFunctionCall(this, context, stage);
    }

    void SetClassification(uint32_t c) { mClassification = c; }
    uint32_t GetClassification() const { return mClassification; }
    void AddClassification(uint32_t c) { mClassification |= c; }
    bool HasClassification(uint32_t c) const { return (mClassification & c) == c; }

   private:
    string mName;
    unordered_map<string, ShaderInputPtr> mInputs;
    unordered_map<string, ShaderOutputPtr> mOutputs;
    ShaderNodeImplPtr mImpl;

    vector<string> mOutputOrder;
    vector<string> mInputOrder;
    uint32_t mClassification;
    ElementPtr mSource;
    ShaderGraphPtr mGraph;
};
}  // namespace Ethereal
