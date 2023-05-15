#pragma once
#include "Core/Asset/Asset.h"
#include "Core/Material/MaterialNode.h"

namespace Ethereal {
using MaterialGraphPtr = Ref<class MaterialGraph>;

class MaterialGraph : public Asset {
   public:
    MaterialGraph();
    ~MaterialGraph();

    void AddNode(MaterialNodePtr node);
    void RemoveNode(NodeID id);
    void AddLink(LinkID id, PinID src, PinID dst);
    void RemoveLink(LinkID id);

    MaterialPinPtr GetPin(PinID id);
    MaterialLinkPtr GetLink(LinkID id);

    bool IsPinLinked(PinID id);

    MaterialNodePtr GetNode(NodeID id);
    MaterialNodePtr GetNode(string name);

    std::string GetName() const { return mName; }
    std::unordered_map<NodeID, MaterialNodePtr>& GetNodes() { return mNodes; }
    std::unordered_map<LinkID, MaterialLinkPtr>& GetLinks() { return mLinks; }

    size_t GenerateID() { return mNextId++; }
    void UpdateLink();

   private:
    std::string mName;
    std::unordered_map<NodeID, MaterialNodePtr> mNodes;
    std::unordered_map<std::string, NodeID> mNameNodeMap;
    std::unordered_map<LinkID, MaterialLinkPtr> mLinks;

   private:
    size_t mNextId = 1;
};
}  // namespace Ethereal