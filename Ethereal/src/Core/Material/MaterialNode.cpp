#include "MaterialNode.h"

#include "Core/Material/MaterialGraph.h"
namespace Ethereal {

MaterialNode::MaterialNode(ElementPtr source, MaterialGraphPtr graph)
    : mSource(source), mGraph(graph) {
    mID = graph->GenerateID();
    Initalize();
}

MaterialPinPtr MaterialNode::AddInput(PinID id, ElementPtr source) {
    if (!source->Is(MaterialElementType::INPUT)) {
        ET_CORE_ASSERT(false, "Only input element can add as node's input");
    }
    MaterialPinPtr pin = MaterialPinPtr::Create(id, source, this);
    mInputs[id] = pin;
    mInputIdMaps[source->GetName()] = id;
    return pin;
}

MaterialPinPtr MaterialNode::AddOutput(PinID id, ElementPtr source) {
    if (!source->Is(MaterialElementType::OUTPUT)) {
        ET_CORE_ASSERT(false, "Only output element can add as node's output");
    }
    MaterialPinPtr pin = MaterialPinPtr::Create(id, source, this);
    mOutputs[id] = pin;
    mOutputIdMaps[source->GetName()] = id;
    return pin;
}

MaterialPinPtr MaterialNode::GetInput(PinID id) {
    auto it = mInputs.find(id);
    if (it != mInputs.end()) {
        return it->second;
    }
    return nullptr;
}

MaterialPinPtr MaterialNode::GetInput(const string& name) {
    auto it = mInputIdMaps.find(name);
    if (it != mInputIdMaps.end()) {
        return GetInput(it->second);
    }
    return nullptr;
}

MaterialPinPtr MaterialNode::GetOutput(PinID id) {
    auto it = mOutputs.find(id);
    if (it != mOutputs.end()) {
        return it->second;
    }
    return nullptr;
}

MaterialPinPtr MaterialNode::GetOutput(const string& name) {
    auto it = mOutputIdMaps.find(name);
    if (it != mOutputIdMaps.end()) {
        return GetOutput(it->second);
    }
    return nullptr;
}

void MaterialNode::Initalize() {
    mName = mSource->GetName();
    mType = NodeType::Blueprint;

    if (mSource->Is(MaterialElementType::NODEINSTANCE)) {
        NodeInstancePtr node = mSource.As<NodeInstance>();
        for (auto& input : node->GetInputs()) {
            AddInput(mGraph->GenerateID(), input);
        }

        for (auto& output : node->GetOutputs()) {
            AddOutput(mGraph->GenerateID(), output);
        }
    } else if (mSource->Is(MaterialElementType::NODEGRAPH)) {
        NodeGraphPtr node = mSource.As<NodeGraph>();
        for (auto& input : node->GetInputs()) {
            AddInput(mGraph->GenerateID(), input);
        }
        for (auto& output : node->GetOutputs()) {
            AddOutput(mGraph->GenerateID(), output);
        }
    } else if (mSource->Is(MaterialElementType::OUTPUT)) {
        InputSocketPtr input = mSource.As<InputSocket>();
        AddOutput(mGraph->GenerateID(), input);
    } else if (mSource->Is(MaterialElementType::INPUT)) {
        OutputSocketPtr output = mSource.As<OutputSocket>();
        AddInput(mGraph->GenerateID(), output);
    }
}

MaterialPin::MaterialPin(PinID id, ElementPtr source, MaterialNodePtr parent)
    : mID(id), mSource(source), mParent(parent) {
    Initalize();
}

void MaterialPin::Initalize() {
    ET_CORE_ASSERT(mSource, "Should set source element before initilize pin");
    bool isInput = mSource->Is(MaterialElementType::INPUT);

    mName = mSource->GetName();
    mKind = isInput ? PinKind::Input : PinKind::Output;
    string typestring = isInput ? mSource.As<NodeInput>()->GetTypeAttribute()
                                : mSource.As<NodeOutput>()->GetTypeAttribute();
    if (typestring == "integer")
        mType = MaterialPinType::Int;
    else if (typestring == "boolean")
        mType = MaterialPinType::Bool;
    else if (typestring == "float")
        mType = MaterialPinType::Float;
    else if (typestring == "float2")
        mType = MaterialPinType::Float2;
    else if (typestring == "float3")
        mType = MaterialPinType::Float3;
    else if (typestring == "float4")
        mType = MaterialPinType::Float4;
    else if (typestring == "color3")
        mType = MaterialPinType::Color3;
    else if (typestring == "color4")
        mType = MaterialPinType::Color4;
    else if (typestring == "surfaceshader" || typestring == "material")
        mType = MaterialPinType::Flow;
    else
        mType = MaterialPinType::Object;
    mAcceptType = mType;
}

MaterialLink::MaterialLink(LinkID id, PinID src, PinID dst, NodeInputPtr sinput,
                           NodeOutputPtr soutput)
    : mID(id), mSrcID(src), mDstID(dst), mSourceInput(sinput), mSourceOutput(soutput) {
    mSourceInput->SetConnector(mSourceOutput);
    if (!mSourceOutput->IsSocket()) {
        mSourceInput->SetAttribute(MaterialAttribute::CONNECTOR,
                                   mSourceOutput->GetParent()->GetName());
        mSourceInput->SetAttribute(MaterialAttribute::PORT, mSourceOutput->GetName());
    } else {
        mSourceInput->SetAttribute(MaterialAttribute::CONNECTOR, mSourceOutput->GetName());
    }
}

}  // namespace Ethereal
