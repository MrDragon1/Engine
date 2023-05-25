#include "ShaderNode.h"
#include "Core/Material/ShaderGenerater/ShaderGraph.h"
#include "Core/Material/ShaderGenerater/Nodes/SourceCodeShaderNode.h"
#include "Core/Material/ShaderGenerater/Nodes/AggregationShaderNode.h"
#include "Core/Material/ShaderGenerater/Nodes/CompoundShaderNode.h"

namespace Ethereal {
ShaderNode::ShaderNode(ElementPtr node, ShaderGraphPtr graph) : mSource(node), mGraph(graph) {
    mName = node->GetName();
    Initalize();
}

void ShaderNode::AddInput(NodeInputPtr input) {
    if (GetInput(input->GetName())) {
        ET_CORE_WARN("ShaderInput {} already exists!", input->GetName());
        return;
    }
    mInputs[input->GetName()] = ShaderInputPtr::Create(this, input);
}

void ShaderNode::AddOutput(NodeOutputPtr output) {
    if (GetOutput(output->GetName())) {
        ET_CORE_WARN("ShaderInput {} already exists!", output->GetName());
        return;
    }
    mOutputs[output->GetName()] = ShaderOutputPtr::Create(this, output);
}

ShaderInputPtr ShaderNode::GetInput(const string& name) {
    auto it = mInputs.find(name);
    if (it != mInputs.end()) {
        return it->second;
    }
    return nullptr;
}

ShaderOutputPtr ShaderNode::GetOutput(const string& name) {
    if (name == EMPTY_STRING) {
        return mOutputs.begin()->second;
    }
    auto it = mOutputs.find(name);
    if (it != mOutputs.end()) {
        return it->second;
    }
    return nullptr;
}

void ShaderNode::Initalize() {
    ET_CORE_ASSERT(mSource, "Empty source node!");
    if (mSource->Is(MaterialElementType::INPUT)) {
    } else if (mSource->Is(MaterialElementType::OUTPUT)) {
    } else if (mSource->Is(MaterialElementType::NODEINSTANCE)) {
        NodeInstancePtr instance = mSource.As<NodeInstance>();
        for (auto& input : instance->GetInputs()) {
            AddInput(input);
        }

        for (auto& output : instance->GetOutputs()) {
            AddOutput(output);
        }

        NodeImplPtr impl = instance->GetNodeImpl();
        if (impl && impl->IsNodeGraph()) {
            mImpl = CompoundShaderNodePtr::Create(impl);
            ShaderGraphPtr graph = mImpl.As<CompoundShaderNode>()->GetGraph();
            for (auto& [name, input] : GetInputs()) {
                input->SetConnector(graph->GetInputSocket(name));
            }

            for (auto& [name, output] : GetOutputs()) {
                output->AddConnector(graph->GetOutputSocket(name));  // this is one-way link
            }
        } else {
            mImpl = SourceCodeShaderNodePtr::Create(impl);
        }

        ET_CORE_ASSERT(!mOutputs.empty(), "Must specify the output of NodeInstance!");
    } else if (mSource->Is(MaterialElementType::NODEGRAPH)) {
        NodeGraphPtr nodeGraph = mSource.As<NodeGraph>();
        for (auto& input : nodeGraph->GetInputs()) {
            AddInput(input);
        }

        for (auto& output : nodeGraph->GetOutputs()) {
            AddOutput(output);
        }

        mImpl = AggregationShaderNodePtr::Create(nodeGraph);
        ET_CORE_ASSERT(!mOutputs.empty(), "Must specify the output of NodeInstance!");

        // connect the input&output to socket in impl.mGraph
        ShaderGraphPtr graph = mImpl.As<AggregationShaderNode>()->GetGraph();
        for (auto& [name, input] : GetInputs()) {
            input->SetConnector(graph->GetInputSocket(name));
        }

        for (auto& [name, output] : GetOutputs()) {
            output->AddConnector(graph->GetOutputSocket(name));  // this is one-way link
        }
    }
}

ShaderInput::ShaderInput(ShaderNodePtr parent, NodeInputPtr input, bool socket)
    : mSource(input), mParent(parent), mIsSocket(socket) {
    mName = mSource->GetName();
}

ShaderInput::ShaderInput(ShaderNodePtr parent, NodeOutputPtr inputSocket, bool socket)
    : mSource(inputSocket), mParent(parent), mIsSocket(socket) {
    mName = mSource->GetName();
}

void ShaderInput::SetConnector(ShaderOutputPtr conn) {
    if (mConnector && conn != mConnector) {
        mConnector->RemoveConnector(this);
    }
    mConnector = conn;
    if (conn) conn->AddConnector(this);
}

ShaderOutput::ShaderOutput(ShaderNodePtr parent, NodeOutputPtr output, bool socket)
    : mSource(output), mParent(parent), mIsSocket(socket) {
    mName = mSource->GetName();
}

ShaderOutput::ShaderOutput(ShaderNodePtr parent, NodeInputPtr outputSocket, bool socket)
    : mSource(outputSocket), mParent(parent), mIsSocket(socket) {
    mName = mSource->GetName();
}

ShaderOutputSocketPtr ShaderOutput::GetConnectorSocket() {
    for (auto iter = mConnectors.begin(); iter != mConnectors.end(); iter++) {
        if ((*iter)->IsSocket()) return *iter;
    }
    return nullptr;
}

void ShaderOutput::AddConnector(ShaderInputPtr conn) {
    for (auto iter = mConnectors.begin(); iter != mConnectors.end(); iter++) {
        if (*iter == conn) return;
    }
    mConnectors.push_back(conn);
}

void ShaderOutput::RemoveConnector(ShaderInputPtr conn) {
    for (auto iter = mConnectors.begin(); iter != mConnectors.end();) {
        if (*iter == conn) {
            iter = mConnectors.erase(iter);
        } else
            iter++;
    }
}

}  // namespace Ethereal
