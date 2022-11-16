#pragma once

#include <glm/glm.hpp>

namespace Ethereal
{
    class OrthographicCamera {
      public:
        OrthographicCamera(float left, float right, float bottom, float top);

        const Vector3& GetPosition() const { return m_Position; }
        void SetPosition(const Vector3& position) {
            m_Position = position;
            RecalculateViewMatrix();
        }
        void SetProjection(float left, float right, float bottom, float top);

        float GetRotation() const { return m_Rotation; }
        void SetRotation(float rotation) {
            m_Rotation = rotation;
            RecalculateViewMatrix();
        }

        const Matrix4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const Matrix4& GetViewMatrix() const { return m_ViewMatrix; }
        const Matrix4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

      private:
        void RecalculateViewMatrix();

      private:
        Matrix4 m_ProjectionMatrix;
        Matrix4 m_ViewMatrix;
        Matrix4 m_ViewProjectionMatrix;

        Vector3 m_Position = {0.0f, 0.0f, 0.0f};
        float m_Rotation = 0.0f;
    };
}  // namespace Ethereal