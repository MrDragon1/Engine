#include "Document.h"

namespace Ethereal {

void Document::ImportLibrary(DocumentPtr lib) {
    if (!lib) return;

    for (auto& child : lib->GetChildren()) {
        if (child->GetType().empty()) {
            ET_CORE_ASSERT(false, "Unable to import element without category");
        }

        const string childName = child->GetName();

        if (GetChild(childName)) continue;

        ElementPtr childCopy = AddChildOfType(child->GetType(), childName);
        childCopy->CopyContentFrom(child);
    }
}

void Document::Validate() {
    Element::Validate();

    for (auto& nd : GetChildren<NodeDefine>(MaterialElementType::NODEDEFINE)) {
        mNodeDefines[nd->GetAttribute(MaterialAttribute::NODEDEF)].push_back(nd);
    }
    for (auto& ni : GetChildren<NodeImpl>(MaterialElementType::NODEIMPL)) {
        mNodeImpls[ni->GetName()] = ni;
    }
    for (auto& ng : GetChildren<NodeGraph>(MaterialElementType::NODEGRAPH)) {
        mNodeGraphs[ng->GetName()] = ng;
    }
    for (auto& ni : GetChildren<NodeInstance>(MaterialElementType::NODEINSTANCE)) {
        mNodeInstances[ni->GetName()] = ni;
    }

    /// Set node define in node instance
    for (auto& [_, ni] : GetNodeInstances()) {
        NodeDefinePtr nd;
        string type = ni->GetAttribute(MaterialAttribute::NODEDEF);
        for (auto& def : GetNodeDefines(type)) {
            bool match = true;
            for (auto& input : def->GetInputs()) {
                NodeInputPtr matchinput = ni->GetChild(input->GetName());

                if (!matchinput) continue;
                if (input->GetTypeAttribute() != matchinput->GetTypeAttribute()) match = false;
            }

            if (match) {
                nd = def;
                break;
            }
        }
        ET_CORE_ASSERT(nd, "Instance {0} has no Node Define.", ni->GetName());
        ni->SetNodeDefine(nd);

        /// Copy all input&output to node instance from node define
        for (auto input : nd->GetInputs()) {
            ElementPtr child = ni->GetChild(input->GetName());
            if (!child || !child->Is(MaterialElementType::INPUT)) {
                auto childCopy = ni->AddChildOfType(MaterialElementType::INPUT, input->GetName());
                childCopy->CopyContentFrom(input);
            }
        }
    }

    /// Set node define in node impl
    for (auto& [name, impl] : mNodeImpls) {
        NodeDefinePtr nd;
        string ndname = impl->GetAttribute(MaterialAttribute::NODEDEF);

        for (auto& [type, defs] : mNodeDefines) {
            if (std::strstr(ndname.c_str(), type.c_str())) {
                for (auto& def : defs) {
                    if (def->GetName() == ndname) {
                        nd = def;
                        break;
                    }
                }
            }
        }

        ET_CORE_ASSERT(nd, "Implementation {0} has no Node Define.", impl->GetName());
        impl->SetNodeDefine(nd);
        nd->AddNodeImpl(impl);
    }

    /// Connect all inputs with CONNECTOR attributes
    for (auto& input : GetInputs()) {
        string conn = input->GetAttribute(MaterialAttribute::CONNECTOR);
        string port = input->GetAttribute(MaterialAttribute::PORT);
        ElementPtr node = GetChild(conn);
        ElementPtr output = node->GetChild(port);
        input->SetConnector(output ? output : node);
    }
    for (auto& [_, node] : GetNodeImpls()) {
        for (auto input : node->GetInputs()) {
            string conn = input->GetAttribute(MaterialAttribute::CONNECTOR);
            string port = input->GetAttribute(MaterialAttribute::PORT);
            ElementPtr node = GetChild(conn);
            ElementPtr output = node->GetChild(port);
            input->SetConnector(output);
        }
    }

    for (auto node : GetChildren()) {
        node->Validate();
    }
}

void NodeInput::SetConnector(ElementPtr conn) {
    if (mConnector && conn != mConnector) {
        mConnector.As<NodeOutput>()->RemoveConnector(this);
        if (conn) conn.As<NodeOutput>()->AddConnector(this);
    }
    mConnector = conn;
}

void NodeInput::Validate() {
    Element::Validate();
    ET_CORE_ASSERT(GetChildren().empty(), "Input port should not have child!");
}

std::vector<ElementPtr> NodeOutput::GetConnectors() {
    std::vector<ElementPtr> res;
    for (auto& [id, elem] : mConnectors) {
        res.push_back(elem);
    }
    return res;
}

void NodeOutput::AddConnector(ElementPtr conn) { mConnectors[conn->GetID()] = conn; }

void NodeOutput::RemoveConnector(ElementPtr conn) {
    auto it = mConnectors.find(conn->GetID());
    if (it != mConnectors.end()) {
        mConnectors.erase(it);
    }
}

void NodeOutput::Validate() {
    Element::Validate();
    ET_CORE_ASSERT(GetChildren().empty(), "Output port should not have child!");
}

void NodeDefine::AddNodeImpl(NodeImplPtr impl) { mNodeImpls[impl->GetID()] = impl; }

vector<NodeImplPtr> NodeDefine::GetNodeImpls() {
    std::vector<NodeImplPtr> res;
    for (auto& [id, elem] : mNodeImpls) {
        res.push_back(elem);
    }
    return res;
}

void NodeDefine::Validate() {
    Element::Validate();
    ET_CORE_ASSERT(!mNodeImpls.empty(), "Node {0} has no implementation!", GetName());
    for (auto node : GetChildren()) {
        node->Validate();
    }
}

void NodeGraph::Validate() {
    Element::Validate();
    for (auto& ni : GetChildren<NodeInstance>(MaterialElementType::NODEINSTANCE)) {
        mNodeInstances[ni->GetName()] = ni;
    }

    DocumentPtr root = GetRoot().As<Document>();
    /// Set node define in node instance
    for (auto& [_, ni] : GetNodeInstances()) {
        NodeDefinePtr nd;
        string type = ni->GetAttribute(MaterialAttribute::NODEDEF);
        for (auto& def : root->GetNodeDefines(type)) {
            bool match = true;
            for (auto& input : def->GetInputs()) {
                NodeInputPtr matchinput = ni->GetChild(input->GetName());

                if (!matchinput) continue;
                if (input->GetTypeAttribute() != matchinput->GetTypeAttribute()) match = false;
            }

            if (match) {
                nd = def;
                break;
            }
        }
        ET_CORE_ASSERT(nd, "Instance {0} has no Node Define.", ni->GetName());
        ni->SetNodeDefine(nd);

        /// Copy all input&output to node instance from node define
        for (auto input : nd->GetInputs()) {
            ElementPtr child = ni->GetChild(input->GetName());
            if (!child || !child->Is(MaterialElementType::INPUT)) {
                auto childCopy = ni->AddChildOfType(MaterialElementType::INPUT, input->GetName());
                childCopy->CopyContentFrom(input);
            }
        }
    }

    for (auto node : GetChildren()) {
        node->Validate();
    }
}

void NodeInstance::Validate() {
    Element::Validate();
    ET_CORE_ASSERT(!GetAttribute(MaterialAttribute::NODEDEF).empty(),
                   "Instance {0} has no \"nodedef\" attribute!", GetName());

    ET_CORE_ASSERT(mNodeDefine, "Instance {0} has no define!", GetName());
    for (auto node : GetChildren()) {
        node->Validate();
    }
}

void NodeImpl::Validate() {
    Element::Validate();
    string file = GetAttribute(MaterialAttribute::FILE);
    string graph = GetAttribute(MaterialAttribute::IMPLGRAPH);
    string code = GetAttribute(MaterialAttribute::SOURCECODE);
    if (!file.empty())
        mImplType = NodeImplType::FILE;
    else if (!graph.empty())
        mImplType = NodeImplType::NODEGRAPH;
    else if (!code.empty())
        mImplType = NodeImplType::INLINE;
    else
        ET_CORE_ASSERT(false, "Implementation {0} is incomplete!", GetName());

    ET_CORE_ASSERT(mNodeDefine, "Implementation {0} has no define!", GetName());
    for (auto node : GetChildren()) {
        node->Validate();
    }
}

}  // namespace Ethereal
