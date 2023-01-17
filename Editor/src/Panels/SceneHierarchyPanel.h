#pragma once
#include "Ethereal.h"
#include "Core/Editor/SelectionManager.h"
#include "Core/Editor/EditorPanel.h"

namespace Ethereal
{
#define COLOR32(R,G,B,A)    (((ImU32)(A)<<24) | ((ImU32)(B)<<16) | ((ImU32)(G)<<8) | ((ImU32)(R)<<0))

    class SceneHierarchyPanel: public EditorPanel {
      public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& scene, SelectionContext selectionContext);

        static SelectionContext GetActiveSelectionContext() { return s_ActiveSelectionContext; }

        void OnImGuiRender(bool& isOpen) override;
        void OnEvent(Event& event) override;

        void SetSceneContext(const Ref<Scene>& scene) override;
        Ref<Scene> GetSceneContext() const { return m_Context; }


      private:
        void ShowInspector();
        void ShowHierarchy();
        void DrawEntityNode(Entity entity);
        void DrawEntityCreateMenu(Entity parent = {});
        template<typename TComponent, typename UIFunction>
        void DrawComponent(const std::string& name, UIFunction uiFunction, Ref<Texture2D> icon = nullptr);
      private:
        Ref<Scene> m_Context;
        SelectionContext m_SelectionContext;

        static SelectionContext s_ActiveSelectionContext;
    };
}  // namespace Ethereal