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
        m_Position = CalculatePosition();

        //        Quaternion orientation = GetOrientation();
        //        m_ViewMatrix = glm::translate(Matrix4::IDENTITY, m_Position) * glm::toMat4(orientation);
        //        m_ViewMatrix = glm::inverse(m_ViewMatrix);

        m_ViewMatrix = Math::LookAt(m_Position, m_Position + GetForwardDirection(), GetUpDirection());
    }

    std::pair<float, float> EditorCamera::PanSpeed() const {
        float x = std::min(GlobalContext::GetViewportSize().x / 1000.0f, 2.4f);  // max = 2.4f
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(GlobalContext::GetViewportSize().y / 1000.0f, 2.4f);  // max = 2.4f
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        return {xFactor, yFactor};
    }

    float EditorCamera::RotationSpeed() const { return 0.8f; }

    float EditorCamera::ZoomSpeed() const {
        float distance = m_Distance * 0.2f;
        distance = std::max(distance, 0.0f);
        float speed = distance * distance;
        speed = std::min(speed, 100.0f);  // max speed = 100
        return speed;
    }

    void EditorCamera::OnUpdate(TimeStamp ts) {
        if (Input::IsKeyPressed(Key::LeftAlt)) {
            const Vector2& mouse{Input::GetMouseX(), Input::GetMouseY()};
            Vector2 delta = (mouse - m_InitialMousePosition) * 0.003f;
            m_InitialMousePosition = mouse;

            if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
                MousePan(delta);
            else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
                MouseRotate(delta);
            else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
                MouseZoom(delta.y);
        }

        UpdateView();
    }

    void EditorCamera::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(ET_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
    }

    bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e) {
        float delta = e.GetYOffset() * 0.1f;
        MouseZoom(delta);
        UpdateView();
        return false;
    }

    void EditorCamera::MousePan(const Vector2& delta) {
        auto [xSpeed, ySpeed] = PanSpeed();
        m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
        m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
    }

    void EditorCamera::MouseRotate(const Vector2& delta) {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
        m_Yaw += yawSign * delta.x * RotationSpeed();
        m_Pitch += delta.y * RotationSpeed();
    }

    void EditorCamera::MouseZoom(float delta) {
        m_Distance -= delta * ZoomSpeed();
        if (m_Distance < 1.0f) {
            m_FocalPoint += GetForwardDirection();
            m_Distance = 1.0f;
        }
    }

    Vector3 EditorCamera::GetUpDirection() const { return Math::Rotate(GetOrientation(), Vector3(0.0f, 1.0f, 0.0f)); }

    Vector3 EditorCamera::GetRightDirection() const { return Math::Rotate(GetOrientation(), Vector3(1.0f, 0.0f, 0.0f)); }

    Vector3 EditorCamera::GetForwardDirection() const { return Math::Rotate(GetOrientation(), Vector3(0.0f, 0.0f, -1.0f)); }

    Vector3 EditorCamera::CalculatePosition() const { return m_FocalPoint - GetForwardDirection() * m_Distance; }

    Quaternion EditorCamera::GetOrientation() const { return Quaternion(Vector3(-m_Pitch, -m_Yaw, 0.0f)); }
}  // namespace Ethereal