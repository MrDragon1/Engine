#include "pch.h"
#include "EditorCamera.h"

#include "Ethereal/src/Base/Input.h"
#include "Ethereal/src/Base/KeyCodes.h"
#include "Ethereal/src/Base/MouseButtonCodes.h"
#include "Ethereal/src/Base/GlobalContext.h"

namespace Ethereal
{
    EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_FOV(fov),
          m_AspectRatio(aspectRatio),
          m_NearClip(nearClip),
          m_FarClip(farClip),
          Camera(Math::Perspective(Math::Radians(fov), aspectRatio, nearClip, farClip)) {
        UpdateView();
    }

    void EditorCamera::UpdateProjection() {
        m_AspectRatio = GlobalContext::GetViewportSize().x / GlobalContext::GetViewportSize().y;
        m_Projection = Math::Perspective(Math::Radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
    }

    void EditorCamera::UpdateView() {
        // m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
        m_ViewMatrix = Math::LookAt(m_Position, m_Position + GetForwardDirection(), GetUpDirection());
    }

    float EditorCamera::RotationSpeed() const { return 0.8f; }

    float EditorCamera::ZoomSpeed() const { return 0.7f; }

    Vector3 EditorCamera::MouseSpeed() const { return Vector3{0.3f, 0.3f, 0.2f}; };

    void EditorCamera::OnUpdate(TimeStamp ts) {
        if (m_Modifying) {
            const Vector2& mouse{Input::GetMouseX(), Input::GetMouseY()};
            Vector2 delta = (mouse - m_InitialMousePosition) * 0.003f;

            if (Input::IsKeyPressed(Key::W)) m_Position += GetForwardDirection() * MouseSpeed().y;
            if (Input::IsKeyPressed(Key::S)) m_Position -= GetForwardDirection() * MouseSpeed().y;

            if (Input::IsKeyPressed(Key::A)) m_Position -= GetRightDirection() * MouseSpeed().x;
            if (Input::IsKeyPressed(Key::D)) m_Position += GetRightDirection() * MouseSpeed().x;

            if (Input::IsKeyPressed(Key::Q)) m_Position -= GetUpDirection() * MouseSpeed().z;
            if (Input::IsKeyPressed(Key::E)) m_Position += GetUpDirection() * MouseSpeed().z;

            MouseRotate(delta);
            Input::SetMousePos(m_InitialMousePosition);
        }

        UpdateView();
    }

    void EditorCamera::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(ET_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
        dispatcher.Dispatch<MouseButtonPressedEvent>(ET_BIND_EVENT_FN(EditorCamera::OnMouseButtonPressed));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(ET_BIND_EVENT_FN(EditorCamera::OnMouseButtonReleased));
    }

    bool EditorCamera::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
        if (e.GetMouseButton() == Mouse::ButtonRight) {
            m_Modifying = true;
            m_InitialMousePosition = {Input::GetMouseX(), Input::GetMouseY()};
            Input::SetMouseInvisible();
        }
        return false;
    }

    bool EditorCamera::OnMouseButtonReleased(MouseButtonReleasedEvent& e) {
        if (e.GetMouseButton() == Mouse::ButtonRight) {
            m_Modifying = false;
            Input::SetMouseVisible();
        }
        return false;
    }

    bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e) {
        MouseZoom(e.GetYOffset());
        UpdateView();
        return false;
    }

    void EditorCamera::MouseRotate(const Vector2& delta) {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
        m_Yaw += yawSign * delta.x * RotationSpeed();
        m_Pitch += delta.y * RotationSpeed();
    }

    void EditorCamera::MouseZoom(float delta) {
        m_FOV -= delta * ZoomSpeed();
        m_FOV = std::clamp(m_FOV, 10.0f, 60.0f);
        UpdateProjection();
    }

    Vector3 EditorCamera::GetUpDirection() const { return Math::Rotate(GetOrientation(), Vector3(0.0f, 1.0f, 0.0f)); }

    Vector3 EditorCamera::GetRightDirection() const { return Math::Rotate(GetOrientation(), Vector3(1.0f, 0.0f, 0.0f)); }

    Vector3 EditorCamera::GetForwardDirection() const { return Math::Rotate(GetOrientation(), Vector3(0.0f, 0.0f, -1.0f)); }

    Quaternion EditorCamera::GetOrientation() const { return Quaternion(Vector3(-m_Pitch, -m_Yaw, 0.0f)); }

}  // namespace Ethereal