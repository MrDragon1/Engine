#pragma once
#include "Core/Editor/GraphEditor.h"
#include "Core/Material/MaterialBase/Document.h"

#include <imgui_node_editor/imgui_node_editor.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace Ethereal {
using MaterialGraphPtr = Ref<class MaterialGraph>;

using MaterialPinPtr = Ref<class MaterialPin>;
using MaterialNodePtr = Ref<class MaterialNode>;
using MaterialLinkPtr = Ref<class MaterialLink>;

class MaterialPin : public RefCounted {
   public:
    PinID mID;
    string mName;
    MaterialPinType mType;
    MaterialPinType mAcceptType;
    PinKind mKind;

    MaterialNodePtr mParent;
    ElementPtr mSource;

    MaterialPin(PinID id, ElementPtr source, MaterialNodePtr parent);

   private:
    void Initalize();
};
class MaterialNode : public RefCounted {
   public:
    MaterialNode() = default;
    ~MaterialNode() = default;

    MaterialNode(ElementPtr source, MaterialGraphPtr graph);

    MaterialPinPtr AddInput(PinID id, ElementPtr source);
    MaterialPinPtr AddOutput(PinID id, ElementPtr source);

    unordered_map<string, MaterialPinPtr> GetInputs() { return mInputs; }
    unordered_map<string, MaterialPinPtr> GetOutputs() { return mOutputs; }
    MaterialPinPtr GetInput(const string& id) { return mInputs[id]; }
    MaterialPinPtr GetOutput(const string& id) { return mOutputs[id]; }

   private:
    void Initalize();

   public:
    NodeID mID;
    std::string mName;
    unordered_map<string, MaterialPinPtr> mInputs;
    unordered_map<string, MaterialPinPtr> mOutputs;
    NodeType mType;

    ElementPtr mSource;
    MaterialGraphPtr mGraph;

    Vector3 mColor;
    Vector2 mSize;
    std::string mState;
    std::string mSavedState;
};

class MaterialLink : public RefCounted {
   public:
    MaterialLink() = default;
    ~MaterialLink() = default;
    LinkID mID;

    PinID mInputID;
    PinID mOutputID;

    ElementPtr mSourceInput;
    ElementPtr mSourceOutput;

    MaterialLink(LinkID id, PinID input, PinID output)
        : mID(id), mInputID(input), mOutputID(output) {}
};

}  // namespace Ethereal
