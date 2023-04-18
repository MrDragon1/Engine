#include "MaterialNode.h"

namespace Ethereal {
MaterialNode& MaterialNode::SetID(NodeID id) {
    ID = id;
    return *this;
}
MaterialNode& MaterialNode::SetName(std::string_view name) {
    Name = name;
    return *this;
}

MaterialNode& MaterialNode::SetType(NodeType type) {
    Type = type;
    return *this;
}

MaterialNode& MaterialNode::Input(PinID id, std::string_view name, MaterialPinType type, MaterialPinType accept) {
    MaterialPin pin{id, name, type, accept == MaterialPinType::Unknown ? type : accept, PinKind::Input};
    Inputs.emplace_back(pin);
    return *this;
}

MaterialNode& MaterialNode::Output(PinID id, std::string_view name, MaterialPinType type) {
    MaterialPin pin{id, name, type, MaterialPinType::Unknown, PinKind::Output};
    Outputs.emplace_back(pin);
    return *this;
}

}  // namespace Ethereal
