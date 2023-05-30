#pragma once
#include "Base/Base.h"
#include "Core/Material/MaterialBase/MaterialDefine.h"

namespace Ethereal {
using DocumentPtr = Ref<class Document>;
using ElementPtr = Ref<class Element>;
using NodeInputPtr = Ref<class NodeInput>;
using NodeOutputPtr = Ref<class NodeOutput>;

class Element : public RefCounted {
   public:
    Element(ElementPtr parent, const string& name, string type = MaterialElementType::ELEMENT)
        : mName(name), mId(UUID()), mParent(parent), mType(type){};

    void CopyContentFrom(ElementPtr source);

    void SetAttribute(const string& attr, const string& value);
    void RemoveAttribute(const string& attr);
    string GetAttribute(const string& attr);

    void AddChild(ElementPtr child);
    ElementPtr AddChildOfType(const string& type, const string& name);
    ElementPtr GetChild(UUID id);
    ElementPtr GetChild(const string& name);
    void RemoveChild(const string& name);

    template <typename T = Element>
    std::vector<Ref<T>> GetChildren(string type = EMPTY_STRING);

    std::vector<NodeInputPtr> GetInputs() {
        return GetChildren<NodeInput>(MaterialElementType::INPUT);
    }
    NodeInputPtr GetInput(const string& name) { return GetChild(name).As<NodeInput>(); }
    std::vector<NodeOutputPtr> GetOutputs() {
        return GetChildren<NodeOutput>(MaterialElementType::OUTPUT);
    }
    NodeOutputPtr GetOutput(const string& name) { return GetChild(name).As<NodeOutput>(); }

    UUID GetID() { return mId; }
    ElementPtr GetParent() { return mParent; }
    ElementPtr GetRoot();
    DocumentPtr GetDocument() { return GetRoot().As<Document>(); }
    const string& GetName() { return mName; }
    string GetFullName();
    const string& GetType() { return mType; }

    bool Is(string type);

    virtual void Validate();

   private:
    UUID mId;
    string mName;
    string mType;
    std::unordered_map<string, string> mAttributes;
    vector<ElementPtr> mChildren;
    ElementPtr mParent;
};

template <typename T>
std::vector<Ref<T>> Element::GetChildren(string type) {
    std::vector<Ref<T>> children;
    for (auto& child : mChildren) {
        if (child->Is(type)) children.push_back(child.As<T>());
    }
    return children;
}

}  // namespace Ethereal
