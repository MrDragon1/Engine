#pragma once

#include "Camera.h"
#include "Base/TimeStamp.h"
#include "Base/Math/Vector.h"
#include "Base/Event/Event.h"
#include "Base/Event/MouseEvent.h"

#include <glm/glm.hpp>
#include <Base/Event/KeyEvent.h>

namespace Ethereal
{

    class EditorCamera : public Camera {
      public:
        EditorCamera() = default;
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

        void OnUpdate(TimeStamp ts);
        void OnEvent(Event& e);

        inline void SetViewportSize() { UpdateProjection(); }

        const Matrix4& GetViewMatrix() const { return mViewMatrix; }
        Matrix4 GetViewProjection() const { return mProjection * mViewMatrix; }

        Vector3 GetUpDirection() const;
        Vector3 GetRightDirection() const;
        Vector3 GetForwardDirection() const;
        const Vector3& GetPosition() const { return mPosition; }
        Quaternion GetOrientation() const;

        float GetPitch() const { return mPitch; }
        float GetYaw() const { return mYaw; }

        float GetFOV() const { return mFOV; }
        float GetAspectRatio() const { return mAspectRatio; }
        float GetNearPlane() const { return mNearClip; }
        float GetFarPlane() const { return mFarClip; }

      private:
        void UpdateProjection();
        void UpdateView();

        bool OnMouseScroll(MouseScrolledEvent& e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
        bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);

        void MouseRotate(const Vector2& delta);
        void MouseZoom(float delta);

        float RotationSpeed() const;
        float ZoomSpeed() const;
        Vector3 MouseSpeed() const;

      private:
        float mFOV = 45.0f, mAspectRatio = 1.778f, mNearClip = 0.1f, mFarClip = 1000.0f;

        Matrix4 mViewMatrix;
        Vector3 mPosition = {0.0f, 0.0f, 3.0f};

        Vector2 mInitialMousePosition = {0.0f, 0.0f};

        float mPitch = 0.0f, mYaw = 0.0f;


        bool mModifying = false;
    };

}  // namespace Ethereal