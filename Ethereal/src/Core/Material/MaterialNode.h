#pragma once
#include "Core/Editor/GraphEditor.h"

#include <imgui_node_editor/imgui_node_editor.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace Ethereal {
static size_t gNextId = 1;  // should use common id generator, or it will cause conflicts
static PinID GeneratePinID() { return gNextId++; }
static NodeID GenerateNodeID() { return gNextId++; }
static LinkID GenerateLinkID() { return gNextId++; }

class MaterialNode;

class MaterialPin {
   public:
    PinID ID;
    std::string Name;
    MaterialPinType Type;
    MaterialPinType AcceptType;
    PinKind Kind;
    MaterialNode* ParentNode;

    MaterialPin(PinID id, std::string_view name, MaterialPinType type, MaterialPinType acceptType, PinKind kind)
        : ID(id), ParentNode(nullptr), Name(name), Type(type), AcceptType(acceptType), Kind(kind) {}
};
class MaterialNode {
   public:
    MaterialNode() = default;
    ~MaterialNode() = default;

    MaterialNode& SetID(NodeID id);
    MaterialNode& SetName(std::string_view name);
    MaterialNode& SetType(NodeType type);
    MaterialNode& Input(PinID id, std::string_view name, MaterialPinType type, MaterialPinType accept);
    MaterialNode& Output(PinID id, std::string_view name, MaterialPinType type);

    virtual void Render(){};

   public:
    NodeID ID;
    std::string Name;
    std::vector<MaterialPin> Inputs;
    std::vector<MaterialPin> Outputs;
    NodeType Type;

    Vector3 Color;
    Vector2 Size;
    std::string State;
    std::string SavedState;
};

class MaterialLink {
   public:
    MaterialLink() = default;
    ~MaterialLink() = default;
    LinkID ID;

    PinID InputID;
    PinID OutputID;
    MaterialLink(LinkID id, PinID input, PinID output) : ID(id), InputID(input), OutputID(output) {}
};

}  // namespace Ethereal
