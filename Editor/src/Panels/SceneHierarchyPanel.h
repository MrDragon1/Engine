#pragma once
#include "Ethereal.h"

namespace Ethereal
{
    class SceneHierarchyPanel {
      public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& scene);

        void SetContext(const Ref<Scene>& scene);
        void OnImGuiRender();
        Entity GetSelectedEntity() const { return m_SelectionContext; }
        void SetSelectedEntity(Entity entity);

      private:
        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity);
        void DrawMaterialTable(Ref<MaterialTable> materialTable);
        template <typename T>
        void DisplayAddComponentEntry(const std::string& entryName);

      private:
        Ref<Scene> m_Context;
        Entity m_SelectionContext;
    };
}  // namespace Ethereal