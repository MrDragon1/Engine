#pragma once
#include "Core/Asset/Asset.h"
#include "MaterialNode.h"

#include "Nodes/Texture.h"
namespace Ethereal {

class MaterialGraph : public Asset {
   public:
    MaterialGraph();
    ~MaterialGraph();

    MaterialGraph& SetName(std::string_view name);
    void AddNode(MaterialNode node);
    void RemoveNode(NodeID id);
    void AddLink(LinkID id, PinID src, PinID dst);
    void RemoveLink(LinkID id);

    MaterialPin* GetPin(PinID id);
    MaterialLink* GetLink(LinkID id);

    bool IsPinLinked(PinID id);

    MaterialNode& GetNode(NodeID id) { return mNodes.at(id); }
    MaterialNode& GetNode(std::string name) { return mNodes.at(mNameNodeMap.at(name)); }

    std::string GetName() const { return mName; }
    std::unordered_map<NodeID, MaterialNode>& GetNodes() { return mNodes; }
    std::unordered_map<LinkID, MaterialLink>& GetLinks() { return mLinks; }

   private:
    std::string mName;
    std::unordered_map<NodeID, MaterialNode> mNodes;
    std::unordered_map<std::string, NodeID> mNameNodeMap;
    std::unordered_map<LinkID, MaterialLink> mLinks;
};
}  // namespace Ethereal