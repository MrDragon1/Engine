#pragma once
#include "Core/Material/MaterialBase/Element.h"
namespace Ethereal {
using DocumentPtr = Ref<class Document>;
using NodeGraphPtr = Ref<class NodeGraph>;
using NodeInstancePtr = Ref<class NodeInstance>;
using NodeDefinePtr = Ref<class NodeDefine>;
using NodeImplPtr = Ref<class NodeImpl>;
using NodeInputPtr = Ref<class NodeInput>;
using NodeOutputPtr = Ref<class NodeOutput>;

class Document : public Element {
   public:
    Document(ElementPtr parent, const string& name)
        : Element(parent, name, MaterialElementType::DOCUMENT){};

    vector<NodeDefinePtr> GetNodeDefines(const string& node) { return mNodeDefines[node]; }
    unordered_map<string, NodeImplPtr> GetNodeImpls() { return mNodeImpls; }
    unordered_map<string, NodeInstancePtr> GetNodeInstances() { return mNodeInstances; }
    unordered_map<string, NodeGraphPtr> GetNodeGraphs() { return mNodeGraphs; }

    NodeImplPtr GetNodeImpl(const string& node) { return mNodeImpls[node]; }
    NodeInstancePtr GetNodeInstance(const string& node) { return mNodeInstances[node]; }
    NodeGraphPtr GetNodeGraph(const string& node) { return mNodeGraphs[node]; }

    void ImportLibrary(DocumentPtr lib);

    void Validate() override;

   private:
    unordered_map<string, vector<NodeDefinePtr>> mNodeDefines;
    unordered_map<string, NodeImplPtr> mNodeImpls;
    unordered_map<string, NodeInstancePtr> mNodeInstances;
    unordered_map<string, NodeGraphPtr> mNodeGraphs;
};

class NodeGraph : public Element {
   public:
    NodeGraph(ElementPtr parent, const string& name)
        : Element(parent, name, MaterialElementType::NODEGRAPH){};

    unordered_map<string, NodeInstancePtr> GetNodeInstances() { return mNodeInstances; }
    NodeInstancePtr GetNodeInstance(const string& node) { return mNodeInstances[node]; }

    void Validate() override;

   private:
    unordered_map<string, NodeInstancePtr> mNodeInstances;
};

class NodeInstance : public Element {
   public:
    NodeInstance(ElementPtr parent, const string& name)
        : Element(parent, name, MaterialElementType::NODEINSTANCE){};

    void SetNodeDefine(NodeDefinePtr nd) { mNodeDefine = nd; }
    NodeDefinePtr GetNodeDefine() { return mNodeDefine; }

    void Validate() override;

   private:
    NodeDefinePtr mNodeDefine;
};

class NodeDefine : public Element {
   public:
    NodeDefine(ElementPtr parent, const string& name)
        : Element(parent, name, MaterialElementType::NODEDEFINE){};

    void AddNodeImpl(NodeImplPtr impl);
    std::vector<NodeImplPtr> GetNodeImpls();

    void Validate() override;

   private:
    std::unordered_map<UUID, NodeImplPtr> mNodeImpls;
};

class NodeImpl : public Element {
   public:
    NodeImpl(ElementPtr parent, const string& name)
        : Element(parent, name, MaterialElementType::NODEIMPL){};

    enum class NodeImplType {
        INLINE,
        FILE,
        NODEGRAPH,
    };

    void SetNodeDefine(NodeDefinePtr nd) { mNodeDefine = nd; }
    NodeDefinePtr GetNodeDefine() { return mNodeDefine; }

    void Validate() override;

   private:
    NodeDefinePtr mNodeDefine;
    NodeImplType mImplType;
};

class NodeInput : public Element {
   public:
    NodeInput(ElementPtr parent, const string& name)
        : Element(parent, name, MaterialElementType::INPUT){};

    ElementPtr GetConnector() { return mConnector; }
    void SetConnector(ElementPtr conn);

    string GetTypeAttribute() { return GetAttribute(MaterialAttribute::TYPE); }

    void Validate() override;

   private:
    ElementPtr mConnector;
};

class NodeOutput : public Element {
   public:
    NodeOutput(ElementPtr parent, const string& name)
        : Element(parent, name, MaterialElementType::OUTPUT){};

    std::vector<ElementPtr> GetConnectors();
    void AddConnector(ElementPtr conn);
    void RemoveConnector(ElementPtr conn);

    string GetTypeAttribute() { return GetAttribute(MaterialAttribute::TYPE); }

    void Validate() override;

   private:
    std::unordered_map<UUID, ElementPtr> mConnectors;
};

}  // namespace Ethereal