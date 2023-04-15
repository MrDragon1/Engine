#include "PanelManager.h"

namespace Ethereal {
PanelManager::~PanelManager() {
    for (auto& m : mPanels) {
        m.clear();
    }
}

void PanelManager::OnImGuiRender() {
    for (auto& chanel : mPanels) {
        for (auto& panel : chanel) {
            if (panel.second.IsOpen) {
                panel.second.Panel->OnImGuiRender(panel.second.IsOpen);
            }
        }
    }
}

void PanelManager::OnEvent(Event& e) {
    for (auto& panelMap : mPanels) {
        for (auto& [id, panelData] : panelMap) panelData.Panel->OnEvent(e);
    }
}

void PanelManager::SetSceneContext(const Ref<Scene>& context) {
    for (auto& panelMap : mPanels) {
        for (auto& [id, panelData] : panelMap) panelData.Panel->SetSceneContext(context);
    }
}

void PanelManager::OnProjectChanged(const Ref<Project>& project) {
    for (auto& panelMap : mPanels) {
        for (auto& [id, panelData] : panelMap) panelData.Panel->OnProjectChanged(project);
    }
}

template <typename TPanel>
Ref<TPanel> PanelManager::GetPanel(const char* strID) {
    static_assert(std::is_base_of<EditorPanel, TPanel>::value, "PanelManager::AddPanel requires TPanel to inherit from EditorPanel");

    uint32_t id = Math::Hash::GenerateFNVHash(strID);

    for (const auto& panelMap : mPanels) {
        if (panelMap.find(id) == panelMap.end()) continue;

        return panelMap.at(id).Panel.As<TPanel>();
    }

    ET_CORE_ERROR("PanelManager::GetPanel: Panel with ID {0} not found", strID);
    return nullptr;
}

void PanelManager::RemovePanel(const char* strID) {
    uint32_t id = Math::Hash::GenerateFNVHash(strID);
    for (auto& panelMap : mPanels) {
        if (panelMap.find(id) == panelMap.end()) continue;

        panelMap.erase(id);
        return;
    }

    ET_CORE_ERROR("PanelManager::RemovePanel: Panel with ID {0} not found", strID);
}

}  // namespace Ethereal
