#pragma once
#include <Core/Editor/SelectionManager.h>
#include "Ethereal.h"

namespace Ethereal
{
    class SceneHierarchyPanel {
      public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& scene);

        void SetContext(const Ref<Scene>& scene);
        void OnImGuiRender();
        Entity GetSelectedEntity() {
            const std::vector<UUID> entities = SelectionManager::GetSelections(s_ActiveSelectionContext);
            if(!entities.empty()) return m_Context->GetEntityWithUUID(entities[0]);
            return Entity();
        }
        void SetSelectedEntity(Entity entity);

      private:
        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity);
        void DrawMaterialTable(Ref<MaterialTable> materialTable);
        template <typename T>
        void DisplayAddComponentEntry(const std::string& entryName);
        void DrawEntityCreateMenu(Entity parent = {});

      private:
        Ref<Scene> m_Context;
        SelectionContext m_SelectionContext;

        bool m_IsWindowFocused = false;
        int32_t m_FirstSelectedRow = -1;
        int32_t m_LastSelectedRow = -1;

        static SelectionContext s_ActiveSelectionContext;
    };
}  // namespace Ethereal