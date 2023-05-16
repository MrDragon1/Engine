#include "MaterialGraph.h"

namespace Ethereal {
MaterialGraph::~MaterialGraph() {}

void MaterialGraph::AddNode(MaterialNodePtr node) {
    if (mNodes.find(node->mID) == mNodes.end()) {
        mNodes[node->mID] = node;
        mNameNodeMap[node->mName] = node->mID;
    } else
        ET_CORE_WARN("Node with {0} already exists!", node->mID);
}

void MaterialGraph::RemoveNode(NodeID id) { mNodes.erase(id); }

void MaterialGraph::AddLink(LinkID id, PinID src, PinID dst) {
    if (mLinks.find(id) == mLinks.end()) {
        mLinks[id] = MaterialLinkPtr::Create(id, src, dst);
    } else
        ET_CORE_WARN("Link with {0} already exists!", id);
}

void MaterialGraph::RemoveLink(LinkID id) { mLinks.erase(id); }

MaterialPinPtr MaterialGraph::GetPin(PinID id) {
    for (auto& [nodeid, node] : mNodes) {
        for (auto& [_, pin] : node->mInputs)
            if (pin->mID == id) return pin;

        for (auto& [_, pin] : node->mOutputs)
            if (pin->mID == id) return pin;
    }
    ET_CORE_ASSERT(false, "Pin with {0} does not exist!", id);
    return nullptr;
}

MaterialLinkPtr MaterialGraph::GetLink(LinkID id) {
    for (auto& [linkid, link] : mLinks)
        if (link->mID == id) return link;

    ET_CORE_ASSERT(false, "Link with {0} does not exist!", id);
    return nullptr;
}

bool MaterialGraph::IsPinLinked(PinID id) {
    for (auto& link : mLinks) {
        if (link.second->mInputID == id || link.second->mOutputID == id) return true;
    }
    return false;
}

MaterialNodePtr MaterialGraph::GetNode(NodeID id) {
    auto it = mNodes.find(id);
    if (it != mNodes.end()) {
        return it->second;
    }
    return nullptr;
}

MaterialNodePtr MaterialGraph::GetNode(string name) {
    auto it = mNameNodeMap.find(name);
    if (it != mNameNodeMap.end()) {
        return GetNode(it->second);
    }
    return nullptr;
}

void MaterialGraph::UpdateLink() {
    mLinks.clear();

    for (auto& [_, node] : mNodes) {
        for (auto& [_, input] : node->GetInputs()) {
            if (input->mSource->Is(MaterialElementType::INPUT)) {
                NodeInputPtr sinput = input->mSource.As<NodeInput>();
                PinID upsteamID;
                if (sinput->GetConnector() &&
                    sinput->GetConnector()->Is(MaterialElementType::OUTPUT)) {
                    // input socket or the output of node instance
                    NodeOutputPtr connector = sinput->GetConnector().As<NodeOutput>();
                    string upNodeName = connector->GetParent()->GetName();
                    MaterialNodePtr upstreamUINode;
                    if (upNodeName == GetName())  // upstream node connect to input socket
                    {
                        upstreamUINode = GetNode(connector->GetName());
                    } else {
                        upstreamUINode = GetNode(upNodeName);
                    }
                    upsteamID = upstreamUINode->GetOutput(connector->GetName())->mID;

                    AddLink(GenerateID(), upsteamID, input->mID);
                }
            }
        }
    }
}

}  // namespace Ethereal
