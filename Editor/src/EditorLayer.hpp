#pragma once

#include "Engine.hpp"
#include "Panels/SceneHierarchyPanel.hpp"
#include "Engine/Renderer/EditorCamera.hpp"
namespace Engine
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
        void SaveSceneAs();

      private:
        OrthographicCameraController m_CameraController;
        EditorCamera m_EditorCamera;
        // Temp
        Ref<VertexArray> m_SquareVA;
        Ref<Shader> m_FlatColorShader;
        Ref<Framebuffer> m_Framebuffer;

        Ref<Texture2D> m_CheckerboardTexture;

        Ref<Scene> m_Scene;
        Entity m_SquareEntity;
        Entity m_CameraEntity;
        Entity m_SecondCamera;
        bool m_PrimaryCamera = true;
        Entity m_HoveredEntity;

        glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.0f};

        glm::vec2 m_ViewportSize = {0.0f, 0.0f};
        glm::vec2 m_ViewportBounds[2];

        bool m_ViewportFocused = false, m_ViewportHovered = false;

        SceneHierarchyPanel m_SceneHierarchyPanel;

        int m_GizmoType = -1;
    };
}  // namespace Engine