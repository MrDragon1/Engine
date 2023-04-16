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

}  // namespace Ethereal
