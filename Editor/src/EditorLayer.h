#pragma once

#include "Ethereal.h"
#include "Core/Editor/PanelManager.h"

#include "Core/Renderer/EditorCamera.h"
#include "Base/Ref.h"

namespace Ethereal
{
    class EditorLayer : public Layer {
      public:
        EditorLayer();
        ~EditorLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;

        void OnUpdate(TimeStamp ts) override;
        void OnImGuiRender() override;
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
        EditorCamera m_EditorCamera;

        Scope<PanelManager> m_PanelManager;

        Ref<Texture2D> m_IconPlay;
        Ref<Texture2D> m_IconStop;

        Ref<Scene> m_RuntimeScene, m_EditorScene, m_CurrentScene;

        std::filesystem::path m_EditorScenePath;

        Entity m_HoveredEntity;

        Vector2 m_ViewportBounds[2];

        bool m_ViewportFocused = false, m_ViewportHovered = false;

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
        Vector3 m_LightPos = Vector3(-20, 40, -10);  // as far as possiable (away from the scene to avoid clamp shadow)
    };
}  // namespace Ethereal