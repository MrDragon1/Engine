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

    const MaterialNode& GetNode(NodeID id) const { return mNodes.at(id); }
    const MaterialNode& GetNode(std::string name) const { return mNodes.at(mNameNodeMap.at(name)); }

    std::string GetName() const { return mName; }
    const std::unordered_map<NodeID, MaterialNode>& GetNodes() { return mNodes; }
    const std::unordered_map<LinkID, MaterialLink>& GetLinks() { return mLinks; }

   private:
    std::string mName;
    std::unordered_map<NodeID, MaterialNode> mNodes;
    std::unordered_map<std::string, NodeID> mNameNodeMap;
    std::unordered_map<LinkID, MaterialLink> mLinks;
};
}  // namespace Ethereal