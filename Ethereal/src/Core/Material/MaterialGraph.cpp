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

void MaterialGraph::RemoveNode(NodeID id) {
    if (mNodes.find(id) != mNodes.end()) {
        auto& node = mNodes[id];
        if (node->mSource->Is(MaterialElementType::INPUT)) {
            node->mSource.As<NodeInput>()->Disconnect();
        } else if (node->mSource->Is(MaterialElementType::OUTPUT)) {
            node->mSource.As<NodeOutput>()->Disconnect();
        } else if (!node->mSource->Is(MaterialElementType::DOCUMENT)) {
            for (auto& input : node->mSource->GetInputs()) {
                input->Disconnect();
            }
            for (auto& output : node->mSource->GetOutputs()) {
                output->Disconnect();
            }
        }

        mNameNodeMap.erase(node->mName);
        mNodes.erase(id);
    } else
        ET_CORE_WARN("Node with {0} not found!", id);
}

void MaterialGraph::AddLink(LinkID id, PinID src, PinID dst) {
    if (mLinks.find(id) == mLinks.end()) {
        if (GetPin(src)->mSource->Is(MaterialElementType::INPUT)) {
            std::swap(src, dst);
        }
        mLinks[id] = MaterialLinkPtr::Create(id, src, dst, GetPin(dst)->mSource.As<NodeInput>(),
                                             GetPin(src)->mSource.As<NodeOutput>());
    } else
        ET_CORE_WARN("Link with {0} already exists!", id);
}

void MaterialGraph::RemoveLink(LinkID id) {
    if (mLinks.find(id) != mLinks.end()) {
        auto& link = mLinks[id];
        link->mSourceInput.As<NodeInput>()->Disconnect();
        // we needn't disconnect output, since it's already disconnected.
        mLinks.erase(id);
    } else
        ET_CORE_WARN("Link with {0} already exists!", id);
}

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
        if (link.second->mSrcID == id || link.second->mDstID == id) return true;
    }
    return false;
}

MaterialPinPtr MaterialGraph::GetConnectPin(PinID id) {
    if (!IsPinLinked(id)) return nullptr;
    for (auto& link : mLinks) {
        if (link.second->mSrcID == id) return GetPin(link.second->mDstID);
        if (link.second->mDstID == id) return GetPin(link.second->mSrcID);
    }
    return nullptr;
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

void MaterialGraph::UpdateLayout() {
    vector<NodeID> sortedNodes;

    std::queue<NodeID> q;
    std::map<size_t, int> outdegrees;

    for (auto& [id, link] : mLinks) {
        outdegrees[GetPin(link->mSrcID)->mParent->mID]++;
    }

    for (auto& [id, node] : mNodes) {
        node->mLevel = 0;
        if (outdegrees[id] == 0) {
            q.push(id);
        }
    }

    while (!q.empty()) {
        int nodeId = q.front();
        q.pop();
        sortedNodes.push_back(nodeId);

        MaterialNodePtr node = GetNode(nodeId);

        for (auto& [name, input] : node->GetInputs()) {
            MaterialPinPtr pin = GetConnectPin(input->mID);
            if (!pin) continue;
            MaterialNodePtr startUI = pin->mParent;
            if (!startUI) continue;

            startUI->mLevel = std::max(startUI->mLevel, node->mLevel + 1);

            if (--outdegrees[startUI->mID] == 0) {
                q.push(startUI->mID);
            }
        }
    }

    std::unordered_map<int, float> width;
    std::unordered_map<int, std::vector<MaterialNodePtr>> levelMap;
    for (auto& [id, node] : mNodes) {
        width[node->mLevel] = std::max(width[node->mLevel], node->mSize.x);
        levelMap[node->mLevel].push_back(node);
    }

    // TODO: consider the pin order to set y position
    float currPosx = 0.0f;
    for (int level = 0; level < levelMap.size(); level++) {
        float currPosy = 0.0f;
        currPosx -= width[level] + 60;
        for (MaterialNodePtr node : levelMap[level]) {
            node->mPosition.x = currPosx;
            node->mPosition.y = currPosy;
            currPosy += node->mSize.y + 40;
        }
    }
}

}  // namespace Ethereal
