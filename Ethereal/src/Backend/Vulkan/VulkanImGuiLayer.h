#pragma once
#include "Base/ImGui/ImGuiLayer.h"

namespace Ethereal {
class VulkanImGuiLayer : public ImGuiLayer {
   public:
    VulkanImGuiLayer();
    ~VulkanImGuiLayer();
    void OnAttach() override;
    void OnDetach() override;
    void OnImGuiRender() override;
    void Begin() override;
    void End() override;
    void OnEvent(Event& e) override;
};

}  // namespace Ethereal
