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

    unordered_map<PinID, MaterialPinPtr> GetInputs() { return mInputs; }
    unordered_map<PinID, MaterialPinPtr> GetOutputs() { return mOutputs; }
    MaterialPinPtr GetInput(PinID id);
    MaterialPinPtr GetOutput(PinID id);
    MaterialPinPtr GetInput(const string& name);
    MaterialPinPtr GetOutput(const string& name);

   private:
    void Initalize();

   public:
    NodeID mID;
    std::string mName;
    unordered_map<PinID, MaterialPinPtr> mInputs;
    unordered_map<PinID, MaterialPinPtr> mOutputs;
    unordered_map<string, PinID> mInputIdMaps;
    unordered_map<string, PinID> mOutputIdMaps;

    NodeType mType;

    ElementPtr mSource;
    MaterialGraphPtr mGraph;

    size_t mLevel = 0;
    Vector3 mColor;
    Vector2 mSize;
    Vector2 mPosition;
    std::string mState;
    std::string mSavedState;
};

class MaterialLink : public RefCounted {
   public:
    MaterialLink() = default;
    ~MaterialLink() = default;
    LinkID mID;

    PinID mSrcID;
    PinID mDstID;

    NodeInputPtr mSourceInput;
    NodeOutputPtr mSourceOutput;

    MaterialLink(LinkID id, PinID src, PinID dst, NodeInputPtr sinput, NodeOutputPtr soutput);
};

}  // namespace Ethereal
