#include "ShaderGraph.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {

ShaderGraph::ShaderGraph(const string& name, DocumentPtr document, bool impl)
    : mName(name), mDocument(document), mIsImpl(impl) {}

void ShaderGraph::AddNode(ShaderNodePtr node) {
    auto it = mNodes.find(node->GetName());
    if (it != mNodes.end()) {
        ET_CORE_WARN("ShaderNode {} already exists!", node->GetName());
    }
    mNodes[node->GetName()] = node;
}

ShaderNodePtr ShaderGraph::GetNode(const string& name) {
    auto it = mNodes.find(name);
    if (it != mNodes.end()) {
        return it->second;
    }
    return nullptr;
}

void ShaderGraph::UpdateLink() {
    for (auto& [nodename, node] : mNodes) {
        for (auto& [inputname, input] : node->GetInputs()) {
            if (!input->IsSocket()) {  // skip the input socket
                ElementPtr conn = input->GetSource().As<NodeInput>()->GetConnector();
                if (conn && conn->Is(MaterialElementType::OUTPUT)) {
                    string upNodeName = conn.As<NodeOutput>()->GetParent()->GetName();
                    if (upNodeName == GetName())  // upstream node connect to input socket
                    {
                        input->SetConnector(GetInputSocket(conn->GetName()));
                    } else {
                        input->SetConnector(GetNode(upNodeName)->GetOutput(conn->GetName()));
                    }
                } else if (!conn) {
                    string connector =
                        input->GetSource()->GetAttribute(MaterialAttribute::CONNECTOR);
                    if (connector.empty()) continue;
                    // In impl NodeGraph, if a node port connect to input, the connector will not
                    // set
                    input->SetConnector(GetInputSocket(connector));
                }
            }
        }
    }
    // connect the output socket to upstream node output port
    for (auto& [name, outputsocket] : GetOutputSockets()) {
        string connector = outputsocket->GetSource()->GetAttribute(MaterialAttribute::CONNECTOR);
        if (connector.empty()) continue;
        string port = outputsocket->GetSource()->GetAttribute(MaterialAttribute::PORT);
        outputsocket->SetConnector(GetNode(connector)->GetOutput(port));
    }
}

void ShaderGraph::TopologicalSort() {
    mSortedNodes.clear();
    std::queue<string> q;
    std::map<string, int> indegrees;

    for (auto& [name, node] : mNodes) {
        for (auto& [name, output] : node->GetOutputs()) {
            for (auto& conn : output->GetConnectors()) {
                if (conn->IsSocket()) continue;
                auto parent = conn->GetParent();
                indegrees[parent->GetName()]++;
            }
        }
    }

    for (auto& [name, node] : mNodes) {
        if (indegrees[name] == 0) {
            q.push(name);
        }
    }

    while (!q.empty()) {
        string name = q.front();
        q.pop();
        ShaderNodePtr node = GetNode(name);
        mSortedNodes.push_back(node);
        for (auto& [name, output] : node->GetOutputs()) {
            for (auto& conn : output->GetConnectors()) {
                ShaderNodePtr downstream = conn->GetParent();
                if (!downstream) continue;
                if (--indegrees[downstream->GetName()] == 0) {
                    q.push(downstream->GetName());
                }
            }
        }
    }
}

void ShaderGraph::UpdateVariable() {
    string prefix = GetName() + "_";
    for (auto& [_, node] : mNodes) {
        for (auto& [_, input] : node->GetInputs()) {
            input->SetVariable(prefix + node->GetName() + "_" + input->GetName());
        }
        for (auto& [_, output] : node->GetOutputs()) {
            output->SetVariable(prefix + node->GetName() + "_" + output->GetName());
        }
    }
    for (auto& [_, input] : GetInputSockets()) {
        input->SetVariable(prefix + input->GetName());
    }
    for (auto& [_, output] : GetOutputSockets()) {
        output->SetVariable(prefix + output->GetName());
    }
}

void ShaderGraph::AddOutputSocket(ShaderOutputSocketPtr output) {
    if (GetOutputSocket(output->GetName())) {
        ET_CORE_WARN("ShaderOutputSocket {} already exists!", output->GetName());
        return;
    }
    mOutputSockets[output->GetName()] = output;
}

void ShaderGraph::AddInputSocket(ShaderInputSocketPtr input) {
    if (GetInputSocket(input->GetName())) {
        ET_CORE_WARN("ShaderInputSocket {} already exists!", input->GetName());
        return;
    }
    mInputSockets[input->GetName()] = input;
}

ShaderOutputSocketPtr ShaderGraph::GetOutputSocket(const string& name) {
    auto it = mOutputSockets.find(name);
    if (it != mOutputSockets.end()) {
        return it->second;
    }
    return nullptr;
}

ShaderInputSocketPtr ShaderGraph::GetInputSocket(const string& name) {
    auto it = mInputSockets.find(name);
    if (it != mInputSockets.end()) {
        return it->second;
    }
    return nullptr;
}
}  // namespace Ethereal
