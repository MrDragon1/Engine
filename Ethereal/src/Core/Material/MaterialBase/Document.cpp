#include "Document.h"

#include "Core/Material/MaterialGraph.h"
#include "Core/Material/ShaderGenerator/ShaderGraph.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
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
    for (auto& ni : GetInputs()) {
        auto socket = InputSocketPtr::Create(ni->GetParent(), ni->GetName());
        socket->CopyContentFrom(ni);
        socket->SetValue(ni->GetValue());
        mInputSockets[ni->GetName()] = socket;
    }
    for (auto& ni : GetOutputs()) {
        auto socket = OutputSocketPtr::Create(ni->GetParent(), ni->GetName());
        socket->CopyContentFrom(ni);
        socket->SetValue(ni->GetValue());
        mOutputSockets[ni->GetName()] = socket;
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
        ni->Validate();
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
    for (auto& [_, node] : GetNodeInstances()) {
        for (auto& input : node->GetInputs()) {
            string conn = input->GetAttribute(MaterialAttribute::CONNECTOR);
            if (conn.empty()) continue;
            string port = input->GetAttribute(MaterialAttribute::PORT);
            if (port.empty()) {
                auto output = GetInputSocket(conn);
                output->SetValue(input->GetValue());
            } else {
                ElementPtr child = GetChild(conn);
                auto output = child->GetChild(port);
                input->SetConnector(output);
            }
        }
    }

    /// We think there are no standalone output node in document,
    /// otherwise, will connect the output with CONNECTOR attributes
    /// like we did in NodeGraph::Validate()

    // Order matters
    for (auto& [_, defines] : mNodeDefines) {
        for (auto& node : defines) {
            node->Validate();
        }
    }
    for (auto& [_, node] : GetNodeImpls()) {
        node->Validate();
    }
    for (auto& [_, node] : GetNodeGraphs()) {
        node->Validate();
    }
    for (auto& [_, node] : GetNodeInstances()) {
        node->Validate();
    }
    for (auto& node : GetInputs()) {
        node->Validate();
    }
    for (auto& node : GetOutputs()) {
        node->Validate();
    }
}

MaterialGraphPtr Document::GenerateUIGraph() {
    MaterialGraphPtr uiGraph = MaterialGraphPtr::Create(GetName(), this);

    /// Create UINode with UIPin
    for (auto& [_, node] : GetNodeInstances()) {
        MaterialNodePtr uiNode = MaterialNodePtr::Create(node, uiGraph);
        uiGraph->AddNode(uiNode);
    }

    for (auto& [_, node] : GetNodeGraphs()) {
        if (node->IsImpl()) continue;
        MaterialNodePtr uiNode = MaterialNodePtr::Create(node, uiGraph);
        uiGraph->AddNode(uiNode);
    }

    /// Create Links
    uiGraph->UpdateLink();

    return uiGraph;
}

MaterialGraphPtr Document::GenerateUIGraphFromNodeGraph(NodeGraphPtr ng) {
    MaterialGraphPtr uiGraph = MaterialGraphPtr::Create(ng->GetName(), ng);
    if (ng->IsImpl()) {
        NodeDefinePtr nodeDefine = ng->GetNodeImpl()->GetNodeDefine();
        for (auto& [name, input] : ng->GetInputSockets()) {
            MaterialNodePtr uiNode = MaterialNodePtr::Create(input, uiGraph);
            uiGraph->AddNode(uiNode);
        }

        for (auto& [name, output] : ng->GetOutputSockets()) {
            MaterialNodePtr uiNode = MaterialNodePtr::Create(output, uiGraph);
            uiGraph->AddNode(uiNode);
        }

        for (auto& [_, node] : ng->GetNodeInstances()) {
            MaterialNodePtr uiNode = MaterialNodePtr::Create(node, uiGraph);
            uiGraph->AddNode(uiNode);
        }

        // We think there no nodegraph in nodegraph for now

    } else {
        for (auto& [name, input] : ng->GetInputSockets()) {
            MaterialNodePtr uiNode = MaterialNodePtr::Create(input, uiGraph);
            uiGraph->AddNode(uiNode);
        }

        for (auto& [name, output] : ng->GetOutputSockets()) {
            MaterialNodePtr uiNode = MaterialNodePtr::Create(output, uiGraph);
            uiGraph->AddNode(uiNode);
        }

        for (auto& [_, node] : ng->GetNodeInstances()) {
            MaterialNodePtr uiNode = MaterialNodePtr::Create(node, uiGraph);
            uiGraph->AddNode(uiNode);
        }

        // We think there no nodegraph in nodegraph for now
    }
    uiGraph->UpdateLink();

    return uiGraph;
}

ShaderGraphPtr Document::GenerateShaderGraph(ShaderContextPtr context) {
    ShaderGraphPtr shaderGraph = ShaderGraphPtr::Create(GetName(), this);

    for (auto& [name, input] : GetInputSockets()) {
        ShaderInputSocketPtr shaderInput = ShaderInputSocketPtr::Create(nullptr, input, true);
        shaderGraph->AddInputSocket(shaderInput);
    }

    /// Create ShaderNode
    for (auto& [_, node] : GetNodeInstances()) {
        ShaderNodePtr shaderNode = ShaderNodePtr::Create(node, shaderGraph, context);
        shaderGraph->AddNode(shaderNode);
    }

    for (auto& [_, node] : GetNodeGraphs()) {
        if (node->IsImpl()) continue;
        ShaderNodePtr shaderNode = ShaderNodePtr::Create(node, shaderGraph, context);
        shaderGraph->AddNode(shaderNode);
    }

    /// Create Links
    shaderGraph->UpdateLink();

    shaderGraph->TopologicalSort();

    ShaderNodePtr& lastNode = shaderGraph->GetSortedNodes().back();

    for (auto& [name, output] : lastNode->GetOutputs()) {
        ShaderOutputSocketPtr shaderOutput =
            ShaderOutputSocketPtr::Create(nullptr, output->GetSource().As<NodeOutput>(), true);
        shaderGraph->AddOutputSocket(shaderOutput);
        shaderOutput->SetConnector(output);
    }

    shaderGraph->UpdateVariable();

    return shaderGraph;
}

ShaderGraphPtr Document::GenerateShaderGraphFromNodeGraph(NodeGraphPtr ng,
                                                          ShaderContextPtr context) {
    ShaderGraphPtr shaderGraph = ShaderGraphPtr::Create(ng->GetName(), this, ng->IsImpl());

    /// Create ShaderNode
    for (auto& [_, node] : ng->GetNodeInstances()) {
        ShaderNodePtr shaderNode = ShaderNodePtr::Create(node, shaderGraph, context);
        shaderGraph->AddNode(shaderNode);

        // Emit unconnected node input to uniform variable
        if (!ng->IsImpl()) {
            for (auto& input : node->GetInputs()) {
                if (input->GetConnector()) continue;
                ShaderInputSocketPtr shaderInput =
                    ShaderInputSocketPtr::Create(shaderNode, input, true);
                shaderGraph->AddInputSocket(shaderInput);
                shaderNode->GetInput(input->GetName())->SetConnector(shaderInput);
            }
        }
    }

    // There are no NodeGraph in NodeGraph

    if (ng->IsImpl())  // Compound Node
    {
        NodeDefinePtr nodeDefine = ng->GetNodeImpl()->GetNodeDefine();
        // there are no input in impl.nodegraph, so we look for it in node define
        for (auto& input : nodeDefine->GetInputs()) {
            ShaderInputSocketPtr shaderInput = ShaderInputSocketPtr::Create(nullptr, input, true);
            shaderGraph->AddInputSocket(shaderInput);
        }

        for (auto& output : ng->GetOutputs()) {
            ShaderOutputSocketPtr shaderOutput =
                ShaderOutputSocketPtr::Create(nullptr, output, true);
            shaderGraph->AddOutputSocket(shaderOutput);
        }
    } else {  // Aggregation Node
        for (auto& input : ng->GetInputs()) {
            ShaderInputSocketPtr shaderInput = ShaderInputSocketPtr::Create(nullptr, input, true);
            shaderGraph->AddInputSocket(shaderInput);
        }

        for (auto& output : ng->GetOutputs()) {
            ShaderOutputSocketPtr shaderOutput =
                ShaderOutputSocketPtr::Create(nullptr, output, true);
            shaderGraph->AddOutputSocket(shaderOutput);
        }
    }

    /// Create Links
    shaderGraph->UpdateLink();

    shaderGraph->UpdateVariable();

    shaderGraph->TopologicalSort();

    return shaderGraph;
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
    string value = GetAttribute(MaterialAttribute::VALUE);
    if (!mValue) mValue = ValueBase::CreateValueFromString(value, GetTypeAttribute());
}

vector<ElementPtr> NodeOutput::GetConnectors() {
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

void NodeOutput::Disconnect() {
    for (auto& [_, conn] : mConnectors) {
        conn.As<NodeInput>()->Disconnect();
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

string NodeDefine::GetNodeGroup() { return GetAttribute(MaterialAttribute::NODEGROUP); }

string NodeDefine::GetNodeDefineString() { return GetAttribute(MaterialAttribute::NODEDEF); }

void NodeDefine::Validate() {
    Element::Validate();
    ET_CORE_ASSERT(!mNodeImpls.empty(), "NodeDefine {0} has no implementation!", GetName());
    for (auto node : GetChildren()) {
        node->Validate();
    }
}

void NodeGraph::Validate() {
    Element::Validate();

    for (auto& ni : GetChildren<NodeInstance>(MaterialElementType::NODEINSTANCE)) {
        mNodeInstances[ni->GetName()] = ni;
    }
    for (auto& ni : GetInputs()) {
        auto socket = InputSocketPtr::Create(ni->GetParent(), ni->GetName());
        socket->CopyContentFrom(ni);
        mInputSockets[ni->GetName()] = socket;
    }
    for (auto& ni : GetOutputs()) {
        auto socket = OutputSocketPtr::Create(ni->GetParent(), ni->GetName());
        socket->CopyContentFrom(ni);
        mOutputSockets[ni->GetName()] = socket;
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
        ni->Validate();
    }

    /// Connect all inputs with CONNECTOR attributes
    for (auto& [_, node] : GetNodeInstances()) {
        for (auto input : node->GetInputs()) {
            string conn = input->GetAttribute(MaterialAttribute::CONNECTOR);
            if (conn.empty()) continue;
            string port = input->GetAttribute(MaterialAttribute::PORT);
            if (IsImpl() && port.empty()) continue;  // connect input port in impl_nodegraph,

            ElementPtr output;
            if (port.empty()) {
                output = GetInputSocket(conn);
            } else {
                ElementPtr child = GetChild(conn);
                output = child->GetChild(port);
            }
            input->SetConnector(output);
        }
    }

    for (auto& [_, socket] : GetOutputSockets()) {
        string conn = socket->GetAttribute(MaterialAttribute::CONNECTOR);
        if (conn.empty()) {
            ET_CORE_WARN("Graph {0}'s output {1} is not connected!", GetName(), socket->GetName());
            continue;
        }
        string port = socket->GetAttribute(MaterialAttribute::PORT);
        if (IsImpl() && port.empty()) continue;  // connect input port in impl_nodegraph,

        ElementPtr output;
        if (port.empty()) {
            output = GetInputSocket(conn);
        } else {
            ElementPtr child = GetChild(conn);
            output = child->GetChild(port);
        }
        socket->SetConnector(output);
    }

    for (auto node : GetChildren()) {
        node->Validate();
    }
}

NodeImplPtr NodeInstance::GetNodeImpl() {
    // TODO: match the input&output type between node impl & node instance
    return GetNodeDefine()->GetNodeImpls()[0];
}

void NodeInstance::Validate() {
    Element::Validate();
    ET_CORE_ASSERT(!GetAttribute(MaterialAttribute::NODEDEF).empty(),
                   "Instance {0} has no \"nodedef\" attribute!", GetName());

    ET_CORE_ASSERT(mNodeDefine, "Instance {0} has no define!", GetName());

    /// Copy all input&output to node instance from node define
    for (auto& input : mNodeDefine->GetInputs()) {
        ElementPtr child = GetChild(input->GetName());
        if (!child || !child->Is(MaterialElementType::INPUT)) {
            input->Validate();
            auto childCopy =
                AddChildOfType(MaterialElementType::INPUT, input->GetName()).As<NodeInput>();
            childCopy->CopyContentFrom(input);
            childCopy->SetValue(input->GetValue());
        }
    }

    for (auto& output : mNodeDefine->GetOutputs()) {
        ElementPtr child = GetChild(output->GetName());
        if (!child || !child->Is(MaterialElementType::OUTPUT)) {
            output->Validate();
            auto childCopy =
                AddChildOfType(MaterialElementType::OUTPUT, output->GetName()).As<NodeOutput>();
            childCopy->CopyContentFrom(output);
            childCopy->SetValue(output->GetValue());
        }
    }

    for (auto& node : GetChildren()) {
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
    else if (!graph.empty()) {
        mImplType = NodeImplType::NODEGRAPH;
        GetNodeGraph()->SetNodeImpl(this);
    } else if (!code.empty())
        mImplType = NodeImplType::INLINE;
    else
        mImplType = NodeImplType::DYNAMIC;

    ET_CORE_ASSERT(mNodeDefine, "Implementation {0} has no define!", GetName());
    for (auto node : GetChildren()) {
        node->Validate();
    }
}

NodeGraphPtr NodeImpl::GetNodeGraph() {
    if (mImplType != NodeImplType::NODEGRAPH) return nullptr;
    string graph = GetAttribute(MaterialAttribute::IMPLGRAPH);

    auto elem = GetRoot().As<Document>()->GetChild(graph);
    if (!elem || !elem->Is(MaterialElementType::NODEGRAPH)) {
        ET_CORE_ASSERT(false, "Missing nodegraph {}", graph);
    }
    return elem.As<NodeGraph>();
}

}  // namespace Ethereal
