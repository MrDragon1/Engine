#pragma once

#include "Camera.h"
#include "Base/TimeStamp.h"
#include "Base/Math/Vector.h"
#include "Base/Event/Event.h"
#include "Base/Event/MouseEvent.h"

#include <glm/glm.hpp>

namespace Ethereal
{

    class EditorCamera : public Camera {
      public:
        EditorCamera() = default;
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

        void OnUpdate(TimeStamp ts);
        void OnEvent(Event& e);

        inline float GetDistance() const { return m_Distance; }
        inline void SetDistance(float distance) { m_Distance = distance; }

        inline void SetViewportSize() { UpdateProjection(); }

        const Matrix4& GetViewMatrix() const { return m_ViewMatrix; }
        Matrix4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

        Vector3 GetUpDirection() const;
        Vector3 GetRightDirection() const;
        Vector3 GetForwardDirection() const;
        const Vector3& GetPosition() const { return m_Position; }
        Quaternion GetOrientation() const;

        float GetPitch() const { return m_Pitch; }
        float GetYaw() const { return m_Yaw; }

        float GetFOV() const { return m_FOV; }
        float GetAspectRatio() const { return m_AspectRatio; }
        float GetNearPlane() const { return m_NearClip; }
        float GetFarPlane() const { return m_FarClip; }

      private:
        void UpdateProjection();
        void UpdateView();

        bool OnMouseScroll(MouseScrolledEvent& e);

        void MousePan(const Vector2& delta);
        void MouseRotate(const Vector2& delta);
        void MouseZoom(float delta);

        Vector3 CalculatePosition() const;

        std::pair<float, float> PanSpeed() const;
        float RotationSpeed() const;
        float ZoomSpeed() const;

      private:
        float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

        Matrix4 m_ViewMatrix;
        Vector3 m_Position = {0.0f, 0.0f, 3.0f};
        Vector3 m_FocalPoint = {0.0f, 0.0f, 0.0f};

        Vector2 m_InitialMousePosition = {0.0f, 0.0f};

        float m_CameraTranslationSpeed = 5.0f;

        float m_Distance = 50.0f;
        float m_Pitch = 0.0f, m_Yaw = 0.0f;
    };

}  // namespace Ethereal