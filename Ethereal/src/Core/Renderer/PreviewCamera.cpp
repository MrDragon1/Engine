#include "PreviewCamera.h"
#include "pch.h"

#include "Base/Input.h"
#include "Base/KeyCodes.h"
#include "Base/MouseButtonCodes.h"
#include "Base/GlobalContext.h"

namespace Ethereal {

PreviewCamera::PreviewCamera()
    : Camera(Math::Perspective(Math::Radians(mFOV), mAspectRatio, mNearClip, mFarClip)) {
    mPosition = Vector3(0, 0, 2);
    mLookat = Vector3(0);
    mUp = Vector3(0, 1, 0);
    UpdateView();
}

void PreviewCamera::OnUpdate(TimeStamp ts) {
    if (mModifying) {
        const Vector2& mouse{Input::GetMouseX(), Input::GetMouseY()};

        Vector4 position = Vector4{mPosition, 1.0f};
        Vector4 pivot = Vector4{mLookat, 1.0f};

        float deltaAngleX =
            (2 * Math::PI / mViewportSize.x);  // a movement from left to right = 2*PI = 360 deg
        float deltaAngleY =
            (Math::PI / mViewportSize.y);  // a movement from top to bottom = PI = 180 deg
        float xAngle = (mLastMousePos.x - mouse.x) * deltaAngleX;
        float yAngle = (mLastMousePos.y - mouse.y) * deltaAngleY;

        float cosAngle = Math::Dot(GetViewDir(), mUp);
        if (cosAngle * Math::Sign(yAngle) > 0.99f) yAngle = 0;

        // step 2: Rotate the camera around the pivot point on the first axis.
        Matrix4 rotationMatrixX(1.0f);
        rotationMatrixX = Math::Rotate(rotationMatrixX, xAngle, mUp);
        position = (rotationMatrixX * (position - pivot)) + pivot;

        // step 3: Rotate the camera around the pivot point on the second axis.
        Matrix4 rotationMatrixY(1.0f);
        rotationMatrixY = Math::Rotate(rotationMatrixY, yAngle, GetRightVector());
        Vector3 finalPosition = (rotationMatrixY * (position - pivot)) + pivot;

        // Update the camera view (we keep the same lookat and the same up vector)
        mPosition = finalPosition;
        UpdateView();

        // Update the mouse position for the next rotation
        mLastMousePos = mouse;
    }

    UpdateView();
}

void PreviewCamera::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(ET_BIND_EVENT_FN(PreviewCamera::OnMouseScroll));
    dispatcher.Dispatch<MouseButtonPressedEvent>(
        ET_BIND_EVENT_FN(PreviewCamera::OnMouseButtonPressed));
    dispatcher.Dispatch<MouseButtonReleasedEvent>(
        ET_BIND_EVENT_FN(PreviewCamera::OnMouseButtonReleased));
}

void PreviewCamera::UpdateView() { mViewMatrix = Math::LookAt(GetPosition(), mLookat, mUp); }

void PreviewCamera::UpdateProjection() {
    mAspectRatio = mViewportSize.x / mViewportSize.y;
    mProjection = Math::Perspective(Math::Radians(mFOV), mAspectRatio, mNearClip, mFarClip);
}

bool PreviewCamera::OnMouseScroll(MouseScrolledEvent& e) {
    mZoom += e.GetYOffset() * 0.3;
    return true;
}

bool PreviewCamera::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
    if (e.GetMouseButton() == Mouse::ButtonRight) {
        mModifying = true;
        mLastMousePos = {Input::GetMouseX(), Input::GetMouseY()};
    }
    return false;
}

bool PreviewCamera::OnMouseButtonReleased(MouseButtonReleasedEvent& e) {
    if (e.GetMouseButton() == Mouse::ButtonRight) {
        mModifying = false;
    }
    return false;
}

}  // namespace Ethereal
