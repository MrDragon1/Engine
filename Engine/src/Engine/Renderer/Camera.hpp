#pragma once

#include <glm/glm.hpp>

namespace Engine
{
    class Camera {
      public:
        Camera() = default;
        Camera(const glm::mat4& projection)
            : m_Projection(projection) {}

        const glm::mat4& GetProjection() const { return m_Projection; }

      private:
        glm::mat4 m_Projection;
    };
}  // namespace Engine