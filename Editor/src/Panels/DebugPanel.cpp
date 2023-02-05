#include "DebugPanel.h"
#include "imgui.h"
#include "Base/ImGui/ImGuizmo.h"
#include "Base/ImGui/UI.h"
#include "Base/GlobalContext.h"

namespace Ethereal {
    DebugPanel::DebugPanel() {

    }

    void DebugPanel::OnImGuiRender(bool& isOpen) {
        ImGui::Begin("Debug", &isOpen);
        bool open = UI::ComponentHeader("Cascade Shadow Map");
        static uint32_t CSMSelectLayer = 0;
        if(open){
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
            Ref<Texture3D> shadowmap = GlobalContext::GetRenderSystem().GetCSMRenderPass()->GetData().ShadowMap.As<Texture3D>();
//            Ref<TextureCube> shadowmap = GlobalContext::GetRenderSystem().GetEnv()->IrradianceMap.As<TextureCube>();
            ImGui::Image((ImTextureID)(intptr_t)shadowmap->GetSubTexture(CSMSelectLayer), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();
    }

    void DebugPanel::OnEvent(Ethereal::Event& event) {

    }

}