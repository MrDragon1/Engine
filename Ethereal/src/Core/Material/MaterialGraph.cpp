#include "MaterialGraph.h"

namespace Ethereal {
MaterialGraph::MaterialGraph() {}

MaterialGraph::~MaterialGraph() {}

MaterialGraph& MaterialGraph::SetName(std::string_view name) {
    mName = name;
    return *this;
}

void MaterialGraph::AddNode(MaterialNode node) {
    if (mNodes.find(node.ID) == mNodes.end()) {
        mNodes[node.ID] = node;
        mNameNodeMap[node.Name] = node.ID;
    } else
        ET_CORE_WARN("Node with {0} already exists!", node.ID);
}

void MaterialGraph::RemoveNode(NodeID id) { mNodes.erase(id); }

void MaterialGraph::AddLink(LinkID id, PinID src, PinID dst) {
    if (mLinks.find(id) == mLinks.end()) {
        mLinks[id] = MaterialLink{id, src, dst};
    } else
        ET_CORE_WARN("Link with {0} already exists!", id);
}

void MaterialGraph::RemoveLink(LinkID id) { mLinks.erase(id); }

MaterialPin* MaterialGraph::GetPin(PinID id) {
    for (auto& [nodeid, node] : mNodes) {
        for (auto& pin : node.Inputs)
            if (pin.ID == id) return &pin;

        for (auto& pin : node.Outputs)
            if (pin.ID == id) return &pin;
    }
    ET_CORE_ASSERT(false, "Pin with {0} does not exist!", id);
    return nullptr;
}

MaterialLink* MaterialGraph::GetLink(LinkID id) {
    for (auto& [linkid, link] : mLinks)
        if (link.ID == id) return &link;

    ET_CORE_ASSERT(false, "Link with {0} does not exist!", id);
    return nullptr;
}

bool MaterialGraph::IsPinLinked(PinID id) {
    for (auto& link : mLinks) {
        if (link.second.InputID == id || link.second.OutputID == id) return true;
    }
    return false;
}

}  // namespace Ethereal
