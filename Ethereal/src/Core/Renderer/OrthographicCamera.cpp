#include "pch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Ethereal
{
    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
        : m_ProjectionMatrix(Math::makeOrthographicProjectionMatrix(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(Matrix4x4::IDENTITY) {
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
    void OrthographicCamera::SetProjection(float left, float right, float bottom, float top) {
        m_ProjectionMatrix = Math::makeOrthographicProjectionMatrix(left, right, bottom, top, -1.0f, 1.0f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthographicCamera::RecalculateViewMatrix() {
        Matrix4x4 transform =
            Math::getTranslateMatrix(m_Position) * Math::getRotateMatrix(Radian(Math::degreesToRadians(m_Rotation)), Vector3(0, 0, 1));

        m_ViewMatrix = transform.inverse();
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}  // namespace Ethereal
