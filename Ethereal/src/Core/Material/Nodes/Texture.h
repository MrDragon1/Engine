#pragma once
#include "Core/Material/MaterialNode.h"
namespace Ethereal {
class TextureNode : public MaterialNode {
   public:
    TextureNode(NodeID id, std::string_view name) {
        SetID(id)
            .SetName(name)
            .SetType(NodeType::Blueprint)
            .Input(GeneratePinID(), "Texture", MaterialPinType::Texture, MaterialPinType::Texture)
            .Input(GeneratePinID(), "Float", MaterialPinType::Float, MaterialPinType::Texture)
            .Input(GeneratePinID(), "Flow", MaterialPinType::Flow, MaterialPinType::Texture)
            .Output(GeneratePinID(), "Color", MaterialPinType::RGB)
            .Output(GeneratePinID(), "Float2", MaterialPinType::Float);
    }
    void Render() override { ImGui::Text("Texture"); }
};
}  // namespace Ethereal