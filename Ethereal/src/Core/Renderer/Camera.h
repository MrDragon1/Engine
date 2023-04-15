#pragma once

#include <glm/glm.hpp>

namespace Ethereal
{
    class Camera {
      public:
        Camera() = default;
        virtual ~Camera() = default;
        Camera(const Matrix4& projection) : mProjection(projection) {}

        const Matrix4& GetProjection() const { return mProjection; }

      protected:
        Matrix4 mProjection = Matrix4::IDENTITY;
    };
}  // namespace Ethereal