#pragma once
#include "EditorPanel.h"
#include "Base/Math/Hash.h"

namespace Ethereal {
    struct PanelData
    {
        const char* ID = "";
        const char* Name = "";
        Ref<EditorPanel> Panel = nullptr;
        bool IsOpen = false;
    };
    enum class PanelCategory{
        Edit = 0,
        View,
        COUNT
    };

    class PanelManager {
      public:
        PanelManager() = default;
        ~PanelManager();

        void OnImGuiRender();
        void OnEvent(Event& e);
        void SetSceneContext(const Ref<Scene>& context);
        void OnProjectChanged(const Ref<Project>& project);

        template<typename TPanel>
        Ref<TPanel> GetPanel(const char* strID);

        void RemovePanel(const char* strID);

        template <typename TPanel>
        Ref<TPanel> AddPanel(PanelCategory category, const PanelData& panelData) {
            static_assert(std::is_base_of<EditorPanel, TPanel>::value, "PanelManager::AddPanel only support TPanel derived from EditorPanel");

            auto& panels = mPanels[(size_t)category];

            auto id = Math::Hash::GenerateFNVHash(panelData.ID);
            if(panels.find(id) != panels.end()) {
                ET_CORE_WARN("PanelManager::AddPanel: Panel with ID {0} already exists", panelData.ID);
                return nullptr;
            }
            panels[id] = panelData;
            return panelData.Panel.As<TPanel>();
        }

        template<typename TPanel, typename... TArgs>
        Ref<TPanel> AddPanel(PanelCategory category, const char* strID, bool isOpenByDefault, TArgs&&... args)
        {
            return AddPanel<TPanel>(category, PanelData{ strID, strID, Ref<TPanel>::Create(std::forward<TArgs>(args)...), isOpenByDefault });
        }

        template<typename TPanel, typename... TArgs>
        Ref<TPanel> AddPanel(PanelCategory category, const char* strID, const char* displayName, bool isOpenByDefault, TArgs&&... args)
        {
            return AddPanel<TPanel>(category, PanelData{ strID, displayName, Ref<TPanel>::Create(std::forward<TArgs>(args)...), isOpenByDefault });
        }
        std::unordered_map<uint32_t, PanelData>& GetPanels(PanelCategory category) { return mPanels[(size_t)category]; }
        const std::unordered_map<uint32_t, PanelData>& GetPanels(PanelCategory category) const { return mPanels[(size_t)category]; }

      private:
        std::array<std::unordered_map<uint32_t, PanelData>, (size_t)PanelCategory::COUNT> mPanels;
    };
}

