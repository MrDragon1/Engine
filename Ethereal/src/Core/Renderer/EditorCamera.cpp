#include "pch.h"
#include "EditorCamera.h"

#include "Base/Input.h"
#include "Base/KeyCodes.h"
#include "Base/MouseButtonCodes.h"
#include "Base/GlobalContext.h"

namespace Ethereal {
EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
    : mFOV(fov),
      mAspectRatio(aspectRatio),
      mNearClip(nearClip),
      mFarClip(farClip),
      Camera(Math::Perspective(Math::Radians(fov), aspectRatio, nearClip, farClip)) {
    UpdateView();
}

void EditorCamera::UpdateProjection() {
    mAspectRatio = GlobalContext::GetViewportSize().x / GlobalContext::GetViewportSize().y;
    mProjection = Math::Perspective(Math::Radians(mFOV), mAspectRatio, mNearClip, mFarClip);
}

void EditorCamera::UpdateView() {
    // mYaw = mPitch = 0.0f; // Lock the camera's rotation
    mViewMatrix = Math::LookAt(mPosition, mPosition + GetForwardDirection(), GetUpDirection());
}

float EditorCamera::RotationSpeed() const { return 0.8f; }

float EditorCamera::ZoomSpeed() const { return 0.7f; }

Vector3 EditorCamera::MouseSpeed() const { return Vector3{0.3f, 0.3f, 0.2f}; };

void EditorCamera::OnUpdate(TimeStamp ts) {
    if (mModifying) {
        const Vector2& mouse{Input::GetMouseX(), Input::GetMouseY()};
        Vector2 delta = (mouse - mInitialMousePosition) * 0.003f;

        if (Input::IsKeyPressed(Key::W)) mPosition += GetForwardDirection() * MouseSpeed().y;
        if (Input::IsKeyPressed(Key::S)) mPosition -= GetForwardDirection() * MouseSpeed().y;

        if (Input::IsKeyPressed(Key::A)) mPosition -= GetRightDirection() * MouseSpeed().x;
        if (Input::IsKeyPressed(Key::D)) mPosition += GetRightDirection() * MouseSpeed().x;

        if (Input::IsKeyPressed(Key::Q)) mPosition -= GetUpDirection() * MouseSpeed().z;
        if (Input::IsKeyPressed(Key::E)) mPosition += GetUpDirection() * MouseSpeed().z;

        MouseRotate(delta);
        Input::SetMousePos(mInitialMousePosition);
    }

    UpdateView();
}

void EditorCamera::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(ET_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
    dispatcher.Dispatch<MouseButtonPressedEvent>(
        ET_BIND_EVENT_FN(EditorCamera::OnMouseButtonPressed));
    dispatcher.Dispatch<MouseButtonReleasedEvent>(
        ET_BIND_EVENT_FN(EditorCamera::OnMouseButtonReleased));
}

bool EditorCamera::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
    if (e.GetMouseButton() == Mouse::ButtonRight) {
        mModifying = true;
        mInitialMousePosition = {Input::GetMouseX(), Input::GetMouseY()};
        Input::SetMouseInvisible();
    }
    return false;
}

bool EditorCamera::OnMouseButtonReleased(MouseButtonReleasedEvent& e) {
    if (e.GetMouseButton() == Mouse::ButtonRight) {
        mModifying = false;
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
    mYaw += yawSign * delta.x * RotationSpeed();
    mPitch += delta.y * RotationSpeed();
}

void EditorCamera::MouseZoom(float delta) {
    mFOV -= delta * ZoomSpeed();
    mFOV = std::clamp(mFOV, 10.0f, 60.0f);
    UpdateProjection();
}

Vector3 EditorCamera::GetUpDirection() const {
    return Math::Rotate(GetOrientation(), Vector3(0.0f, 1.0f, 0.0f));
}

Vector3 EditorCamera::GetRightDirection() const {
    return Math::Rotate(GetOrientation(), Vector3(1.0f, 0.0f, 0.0f));
}

Vector3 EditorCamera::GetForwardDirection() const {
    return Math::Rotate(GetOrientation(), Vector3(0.0f, 0.0f, -1.0f));
}

Quaternion EditorCamera::GetOrientation() const {
    return Quaternion(Vector3(-mPitch, -mYaw, 0.0f));
}

}  // namespace Ethereal