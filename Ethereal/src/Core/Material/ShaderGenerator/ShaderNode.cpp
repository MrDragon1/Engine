#include "ShaderNode.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
#include "Core/Material/ShaderGenerator/Nodes/AggregationShaderNode.h"

namespace Ethereal {
ShaderNode::ShaderNode(ElementPtr node, ShaderGraphPtr graph, ShaderContextPtr context)
    : mSource(node), mGraph(graph) {
    mName = node->GetName();
    Initalize(context);
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

void ShaderNode::Initalize(ShaderContextPtr context) {
    ET_CORE_ASSERT(mSource, "Empty source node!");
    if (mSource->Is(MaterialElementType::INPUT)) {
    } else if (mSource->Is(MaterialElementType::OUTPUT)) {
    } else if (mSource->Is(MaterialElementType::NODEINSTANCE)) {
        NodeInstancePtr instance = mSource.As<NodeInstance>();
        NodeDefinePtr nodeDefine = instance->GetNodeDefine();

        for (auto& input : nodeDefine->GetInputs()) {
            mInputOrder.push_back(input->GetName());
        }
        for (auto& output : nodeDefine->GetOutputs()) {
            mOutputOrder.push_back(output->GetName());
        }

        for (auto& name : mInputOrder) {
            NodeInputPtr input = instance->GetInput(name);
            if (input) {
                AddInput(input);
                if (!nodeDefine->GetInput(name)) {
                    ET_CORE_WARN("This input {} is not defined in node {}!", name,
                                 nodeDefine->GetName());
                }
                // Emit unconnected node input (with only value attribute) to uniform variable
                if (!mGraph->IsImpl() &&
                    input->GetAttribute(MaterialAttribute::CONNECTOR).empty() &&
                    !input->GetAttribute(MaterialAttribute::VALUE).empty()) {
                    ShaderInputSocketPtr shaderInput =
                        ShaderInputSocketPtr::Create(this, input, true);
                    mGraph->AddInputSocket(shaderInput);
                    GetInput(input->GetName())->SetConnector(shaderInput);
                }
            } else {
                input = nodeDefine->GetInput(name);
                AddInput(input);
                // Emit override node input to uniform variable
                if (!mGraph->IsImpl()) {
                    ShaderInputSocketPtr shaderInput =
                        ShaderInputSocketPtr::Create(this, input, true);
                    mGraph->AddInputSocket(shaderInput);
                    GetInput(input->GetName())->SetConnector(shaderInput);
                }
            }
        }

        for (auto& name : mOutputOrder) {
            NodeOutputPtr output = instance->GetOutput(name);
            if (output)
                AddOutput(output);
            else
                AddOutput(nodeDefine->GetOutput(name));
        }

        mImpl = context->GetShaderGenerator().GetImpl(instance, context);
        ET_CORE_ASSERT(!mOutputs.empty(), "Must specify the output of NodeInstance!");
    } else if (mSource->Is(MaterialElementType::NODEGRAPH)) {
        NodeGraphPtr nodeGraph = mSource.As<NodeGraph>();
        for (auto& input : nodeGraph->GetInputs()) {
            AddInput(input);
            mInputOrder.push_back(input->GetName());
        }

        for (auto& output : nodeGraph->GetOutputs()) {
            AddOutput(output);
            mOutputOrder.push_back(output->GetName());
        }

        mImpl = AggregationShaderNode::Create();
        mImpl->Initilize(nodeGraph, context);
        ET_CORE_ASSERT(!mOutputs.empty(), "Must specify the output of NodeInstance!");
    }
}

ShaderInput::ShaderInput(ShaderNodePtr parent, NodeInputPtr input, bool socket)
    : ShaderPort(parent, input, socket) {}

ShaderInput::ShaderInput(ShaderNodePtr parent, NodeOutputPtr inputSocket, bool socket)
    : ShaderPort(parent, inputSocket, socket) {}

void ShaderInput::SetConnector(ShaderOutputPtr conn) {
    if (mConnector && conn != mConnector) {
        mConnector->RemoveConnector(this);
    }
    mConnector = conn;
    if (conn) conn->AddConnector(this);
}

ShaderOutput::ShaderOutput(ShaderNodePtr parent, NodeOutputPtr output, bool socket)
    : ShaderPort(parent, output, socket) {}

ShaderOutput::ShaderOutput(ShaderNodePtr parent, NodeInputPtr outputSocket, bool socket)
    : ShaderPort(parent, outputSocket, socket) {}

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

ShaderPort::ShaderPort(ShaderNodePtr parent, ElementPtr source, bool socket /*= false*/)
    : mParent(parent), mSource(source), mIsSocket(socket) {
    mName = source->GetName();
    if (!source->GetAttribute(MaterialAttribute::CHANNELS).empty()) {
        mChannels = "." + source->GetAttribute(MaterialAttribute::CHANNELS);
    }
    string uniform = source->GetAttribute(MaterialAttribute::UNIFORM);
    if (uniform != "false") {
        SetUniform();
    }

    if (source->Is(MaterialElementType::INPUT)) {
        SetValue(source.As<NodeInput>()->GetValue());
    } else if (source->Is(MaterialElementType::OUTPUT)) {
        if (source.As<NodeOutput>()->GetValue()) {
            SetValue(source.As<NodeOutput>()->GetValue());
        } else {
            SetValue(ValueBase::CreateValueWithType(
                source.As<NodeOutput>()->GetAttribute(MaterialAttribute::TYPE)));
        }
    }
};

ShaderPort::ShaderPort(ShaderNodePtr parent, const string& type, const string& name) {
    mIsSocket = false;
    mName = name;
    mValue = ValueBase::CreateValueWithType(type);
    mSource = nullptr;
    mParent = parent;
    mVariable = name;
    mChannels = "";
}

string ShaderPort::GetFullName() {
    string res = mName;
    if (mParent) {
        res = mParent->GetName() + "_" + res;
        // if (mParent->GetGraph()) res = mParent->GetGraph()->GetName() + "_" + res;
    }
    return res;
}

string ShaderNodeImpl::GetInputVariable(ShaderInputPtr input, ShaderContextPtr context) {
    auto output = input->GetConnector();
    string res;
    if (output) {
        res = output->GetVariable(context->GetScope());
    } else if (!input->IsUniform()) {
        res =
            input->GetValue()->GetSyntaxString() + "(" + input->GetValue()->GetValueString() + ")";
    } else {
        res = input->GetVariable(context->GetScope());
    }
    return res + input->GetChannels();
}

}  // namespace Ethereal
