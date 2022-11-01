#pragma once
#include "pch.h"
#include "Animation.h"
namespace Ethereal
{
    // The Joint infomation, not include any animation info
    class Joint : public RefCounted {
      public:
        Joint() = default;

        void SetLocalTransform(AnimState state);

        glm::mat4 GetLocalTransform() { return m_LocalTransform; }

      public:
        Ref<Joint> m_Parent = nullptr;
        std::vector<Ref<Joint>> m_Children;
        std::string m_Name;
        size_t m_ID = -1;

        glm::vec3 m_InitialPosition{glm::vec3(0.0f)};
        glm::quat m_InitialRotation{glm::quat(glm::vec3(0.0f))};
        glm::vec3 m_InitialScale{glm::vec3(1.0f)};

        glm::mat4 m_OffsetMatrix{glm::mat4(1.0f)};
        glm::mat4 m_InverseOffsetMatrix{glm::mat4(1.0f)};
        glm::mat4 m_LocalTransform{glm::mat4(1.0f)};
    };

}  // namespace Ethereal
