#pragma once

#include "Camera.h"
#include "Base/TimeStamp.h"
#include "Base/Math/Vector.h"
#include "Base/Event/Event.h"
#include "Base/Event/MouseEvent.h"

#include <Base/Event/KeyEvent.h>

namespace Ethereal {

class PreviewCamera : public Camera {
   public:
    PreviewCamera();

    void OnUpdate(TimeStamp ts);
    void OnEvent(Event& e);

    void SetViewportSize(Vector2 size) {
        mViewportSize = size;
        UpdateProjection();
    }

    const Matrix4& GetViewMatrix() const { return mViewMatrix; }
    Matrix4 GetViewProjection() const { return mProjection * mViewMatrix; }

    Vector3 GetUpDirection() { return mUp; }
    Vector3 GetForwardDirection() { return mLookat; }
    Vector3 GetPosition() { return mPosition * mZoom; }
    Vector3 GetViewDir() const { return -Math::Transpose(mViewMatrix)[2]; }
    Vector3 GetRightVector() const { return Math::Transpose(mViewMatrix)[0]; }

   private:
    void UpdateView();
    void UpdateProjection();

    bool OnMouseScroll(MouseScrolledEvent& e);
    bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
    bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);

   private:
    Matrix4 mViewMatrix;
    float mFOV = 45.0f, mAspectRatio = 1.778f, mNearClip = 0.1f, mFarClip = 1000.0f;

    Vector3 mPosition, mLookat, mUp;
    float mZoom = 1.0f;
    Vector2 mLastMousePos = {0.0f, 0.0f};
    Vector2 mViewportSize;
    bool mModifying = false;
};

}  // namespace Ethereal
