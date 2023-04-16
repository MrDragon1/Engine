#pragma once
#include "Core/Material/MaterialNode.h"
namespace Ethereal {
class TextureNode : public MaterialNode {
   public:
    TextureNode(NodeID id, std::string_view name) {
        SetID(id)
            .SetName(name)
            .Input(GeneratePinID(), "Texture", MaterialPinType::Texture, MaterialPinType::Texture)
            .Output(GeneratePinID(), "Color", MaterialPinType::RGB);
    }
    void Render() override { ImGui::Text("Texture"); }
};
}  // namespace Ethereal