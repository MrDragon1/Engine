#include "DebugPanel.h"
#include "imgui.h"

#include "Base/GlobalContext.h"
#include "Base/ImGui/ImGuizmo.h"  // must be included after imgui.h
#include "Base/ImGui/UI.h"

namespace Ethereal {
DebugPanel::DebugPanel() {
    auto api = GlobalContext::GetDriverApi();
    mCSMShadowMap =
        api->CreateTexture(1, Project::GetConfigManager().sCSMConfig.ShadowMapSize,
                           Project::GetConfigManager().sCSMConfig.ShadowMapSize, 1,
                           TextureFormat::DEPTH, TextureUsage::DEFAULT, TextureType::TEXTURE_2D);
}

void DebugPanel::OnImGuiRender(bool& isOpen) {
    ImGui::Begin("Debug", &isOpen);
    bool open = UI::ComponentHeader("Cascade Shadow Map");
    static uint32_t CSMSelectLayer = 0;
    if (open) {
        ImGui::Text("Cascade Shadow Map");
        if (ImGui::BeginCombo("CSM Layer", std::to_string(CSMSelectLayer).c_str())) {
            for (int i = 0; i < 5; i++) {
                bool isSelected = CSMSelectLayer == i;
                if (ImGui::Selectable(std::to_string(i).c_str(), isSelected)) {
                    CSMSelectLayer = i;
                }

                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        Ref<Texture> shadowmap = Project::GetConfigManager().sCSMConfig.ShadowMap;
        auto api = GlobalContext::GetDriverApi();
        api->GetSubTexture(shadowmap, CSMSelectLayer, mCSMShadowMap);

        ImGui::Image(api->GetTextueID(mCSMShadowMap), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::End();
}

void DebugPanel::OnEvent(Ethereal::Event& event) {}

}  // namespace Ethereal