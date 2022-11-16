#pragma once

#include <glm/glm.hpp>

namespace Ethereal
{
    class Camera {
      public:
        Camera() = default;
        virtual ~Camera() = default;
        Camera(const Matrix4& projection) : m_Projection(projection) {}

        const Matrix4& GetProjection() const { return m_Projection; }

      protected:
        Matrix4 m_Projection = Matrix4::IDENTITY;
    };
}  // namespace Ethereal