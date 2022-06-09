#pragma once

#include "Core/Renderer/OrthographicCamera.hpp"
#include "Core/Timestep.hpp"

#include "Core/Event/ApplicationEvent.hpp"
#include "Core/Event/MouseEvent.hpp"

namespace Engine
{
    class OrthographicCameraController
    {
    public:
        OrthographicCameraController(float aspectRatio, bool rotation = true);

        void OnUpdate(Timestep ts);
        void OnEvent(Event& e);

        OrthographicCamera& GetCamera() { return m_Camera; }
        const OrthographicCamera& GetCamera() const { return m_Camera; }
    private:
        bool OnMouseScrolled(MouseScrolledEvent& e);
        bool OnWindowResized(WindowResizeEvent& e);
    private:
        float m_AspectRatio;
        float m_ZoomLevel = 1.0f;
        OrthographicCamera m_Camera;

        bool m_Rotation = false;
        glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
        float m_CameraRotation = 0.0f;
        float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
    };
} 