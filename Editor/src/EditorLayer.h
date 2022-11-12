#pragma once

#include "Ethereal.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/MaterialEditPanel.h"

#include "Core/Renderer/EditorCamera.h"
#include "Base/Ref.h"

namespace Ethereal
{
    class EditorLayer : public Layer {
      public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        void OnUpdate(TimeStamp ts) override;
        virtual void OnImGuiRender() override;
        void OnEvent(Event& e) override;

      private:
        bool OnKeyPressed(KeyPressedEvent& e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

        void NewScene();
        void OpenScene();
        void OpenScene(const std::filesystem::path& path);
        void SaveSceneAs();
        void SaveScene();

        void DuplicateEntity();
        void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

        void OnScenePlay();
        void OnSceneStop();

        void UI_Toolbar();
        void ShowSkyboxSettingWindow(bool* p_open);
        void ShowProjectSettingWindow(bool* p_open);

      private:
        OrthographicCameraController;
        EditorCamera m_EditorCamera;

        Ref<Texture2D> m_IconPlay;
        Ref<Texture2D> m_IconStop;

        Ref<Scene> m_ActiveScene;
        Ref<Scene> m_EditorScene;

        std::filesystem::path m_EditorScenePath;

        Entity m_HoveredEntity;

        glm::vec2 m_ViewportBounds[2];

        bool m_ViewportFocused = false, m_ViewportHovered = false;

        SceneHierarchyPanel m_SceneHierarchyPanel;
        ContentBrowserPanel m_ContentBrowserPanel;
        MaterialEditPanel m_MaterialEditPanel;

        int m_GizmoType = -1;

        enum class SceneState
        {
            Edit = 0,
            Play = 1
        };

        SceneState m_SceneState = SceneState::Edit;
        bool b_ShowDemoImGui = false;
        bool b_ShowSkyboxSettings = false;
        bool b_ShowProjectSettings = true;
        glm::vec3 m_LightPos = glm::vec3(-20, 40, -10);  // as far as possiable (away from the scene to avoid clamp shadow)

        RenderSceneData m_RenderSceneData;
    };
}  // namespace Ethereal