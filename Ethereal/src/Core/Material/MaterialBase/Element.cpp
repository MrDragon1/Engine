#include "Element.h"
#include "Core/Material/MaterialBase/Document.h"

namespace Ethereal {

void Element::CopyContentFrom(ElementPtr source) {
    mAttributes = source->mAttributes;

    for (auto child : source->GetChildren()) {
        const string& name = child->GetName();

        if (GetChild(name)) continue;

        ElementPtr childCopy = AddChildOfType(child->GetType(), name);
        childCopy->CopyContentFrom(child);
    }
}

void Element::SetAttribute(const string& attr, const string& value) { mAttributes[attr] = value; }

void Element::RemoveAttribute(const string& attr) {
    auto it = mAttributes.find(attr);
    if (it != mAttributes.end()) {
        mAttributes.erase(it);
    }
}

string Element::GetAttribute(const string& attr) {
    auto it = mAttributes.find(attr);
    if (it != mAttributes.end()) {
        return (*it).second;
    }
    return EMPTY_STRING;
}

void Element::AddChild(ElementPtr child) {
    if (!child) ET_CORE_ASSERT("Try to add an ampty child!");
    if (GetChild(child->GetID()))
        ET_CORE_WARN("Child " + child->GetName() + " already exists");
    else
        mChildren.push_back(child);
}

ElementPtr Element::AddChildOfType(const string& type, const string& name) {
    ElementPtr child;
    if (type == MaterialElementType::DOCUMENT)
        child = Ref<Document>::Create(this, name);
    else if (type == MaterialElementType::INPUT)
        child = Ref<NodeInput>::Create(this, name);
    else if (type == MaterialElementType::OUTPUT)
        child = Ref<NodeOutput>::Create(this, name);
    else if (type == MaterialElementType::NODEDEFINE)
        child = Ref<NodeDefine>::Create(this, name);
    else if (type == MaterialElementType::NODEGRAPH)
        child = Ref<NodeGraph>::Create(this, name);
    else if (type == MaterialElementType::NODEIMPL)
        child = Ref<NodeImpl>::Create(this, name);
    else {
        child = Ref<NodeInstance>::Create(this, name);
        child->SetAttribute(MaterialAttribute::NODEDEF, type);
    }

    AddChild(child);
    return child;
}

ElementPtr Element::GetChild(UUID id) {
    for (auto it = mChildren.begin(); it != mChildren.end(); it++) {
        if ((*it)->GetID() == id) return (*it);
    }
    return nullptr;
}

ElementPtr Element::GetChild(const string& name) {
    for (auto it = mChildren.begin(); it != mChildren.end(); it++) {
        if ((*it)->GetName() == name) return (*it);
    }
    return nullptr;
}

void Element::RemoveChild(const string& name) {
    for (auto it = mChildren.begin(); it != mChildren.end(); it++) {
        if ((*it)->GetName() == name) mChildren.erase(it);
    }
}

ElementPtr Element::GetRoot() {
    ElementPtr elem = this;
    while (elem->GetParent()) elem = elem->GetParent();
    return elem;
}

string Element::GetFullName() {
    string res = mName;
    if (mParent) {
        res = mParent->GetName() + "_" + res;
        // if (mParent->GetGraph()) res = mParent->GetGraph()->GetName() + "_" + res;
    }
    return res;
}

bool Element::Is(string type) {
    if (type.empty()) return true;
    return GetType() == type;
}

void Element::Validate() {
    ET_CORE_ASSERT(mId);
    ET_CORE_ASSERT(!mName.empty());
    ET_CORE_ASSERT(!mType.empty() && mType != MaterialElementType::ELEMENT);
}

}  // namespace Ethereal
