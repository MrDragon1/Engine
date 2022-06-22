#pragma once
#include "Engine.hpp"

namespace Engine
{
    class SceneHierarchyPanel {
      public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& scene);

        void SetContext(const Ref<Scene>& scene);
        void OnImguiRender();

        private:
        void DrawEntityNode(Entity entity);
      private:
        Ref<Scene> m_Context;
        Entity m_SelectionContext;
    };
}  // namespace Engine