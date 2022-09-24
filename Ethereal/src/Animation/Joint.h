#pragma once
#include "pch.h"

namespace Ethereal
{
    // The Joint infomation, not include any animation info
    class Joint {
      public:
        Joint* m_Parent{nullptr};
        std::vector<Joint*> m_Children;
        std::string m_Name;
        size_t m_ID;

        glm::vec3 m_InitialPosition;
        glm::quat m_InitialRotation;
        glm::vec3 m_InitialScale;

        glm::mat4 m_InverseT{glm::mat4(1.0f)};

      public:
        Joint() = default;
    };

}  // namespace Ethereal
