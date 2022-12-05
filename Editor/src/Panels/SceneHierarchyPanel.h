#pragma once
#include <Core/Editor/SelectionManager.h>
#include "Ethereal.h"

namespace Ethereal
{
#define COLOR32(R,G,B,A)    (((ImU32)(A)<<24) | ((ImU32)(B)<<16) | ((ImU32)(G)<<8) | ((ImU32)(R)<<0))

    class SceneHierarchyPanel {
      public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& scene);

        void SetContext(const Ref<Scene>& scene);
        Ref<Scene> GetSceneContext() const { return m_Context; }
        static SelectionContext GetActiveSelectionContext() { return s_ActiveSelectionContext; }

        void OnImGuiRender();
        Entity GetSelectedEntity() {
            const std::vector<UUID> entities = SelectionManager::GetSelections(s_ActiveSelectionContext);
            if (!entities.empty()) return m_Context->GetEntityWithUUID(entities[0]);
            return Entity();
        }
        void SetSelectedEntity(Entity entity);

      private:
        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity);
        void DrawEntityCreateMenu(Entity parent = {});
        template<typename TComponent, typename UIFunction>
        void DrawComponent(const std::string& name, UIFunction uiFunction, Ref<Texture2D> icon = nullptr);
      private:
        Ref<Scene> m_Context;
        SelectionContext m_SelectionContext;

        bool m_IsWindowFocused = false;
        int32_t m_FirstSelectedRow = -1;
        int32_t m_LastSelectedRow = -1;

        static SelectionContext s_ActiveSelectionContext;
    };
}  // namespace Ethereal