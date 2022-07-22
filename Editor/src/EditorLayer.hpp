#pragma once

#include "Ethereal.hpp"
#include "Panels/SceneHierarchyPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"
#include "Ethereal/Renderer/EditorCamera.hpp"

namespace Ethereal
{
    class EditorLayer : public Layer {
      public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        void OnUpdate(Timestep ts) override;
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
      private:
        OrthographicCameraController m_CameraController;
        EditorCamera m_EditorCamera;

        Ref<Texture2D> m_IconPlay;
        Ref<Texture2D> m_IconStop;

        Ref<Scene> m_ActiveScene;
        Ref<Scene> m_EditorScene;

        std::filesystem::path m_EditorScenePath;

        Entity m_HoveredEntity;

        glm::vec2 m_ViewportSize = {0.0f, 0.0f};
        glm::vec2 m_ViewportBounds[2];

        bool m_ViewportFocused = false, m_ViewportHovered = false;

        SceneHierarchyPanel m_SceneHierarchyPanel;
        ContentBrowserPanel m_ContentBrowserPanel;

        int m_GizmoType = -1;

        enum class SceneState
        {
            Edit = 0,
            Play = 1
        };

        SceneState m_SceneState = SceneState::Edit;

        glm::vec3 m_LightPos= glm::vec3(-20,40,-10);// as far as possiable (away from the scene to avoid clamp shadow)
    };
}  // namespace Ethereal