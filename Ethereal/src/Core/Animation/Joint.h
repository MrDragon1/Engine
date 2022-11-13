#pragma once
#include "pch.h"
#include "Core/Animation/Animation.h"
namespace Ethereal
{
    // The Joint infomation, not include any animation info
    class Joint : public RefCounted {
      public:
        Joint() = default;

        void SetLocalTransform(AnimState state);

        Matrix4x4 GetLocalTransform() { return m_LocalTransform; }

      public:
        Ref<Joint> m_Parent = nullptr;
        std::vector<Ref<Joint>> m_Children;
        std::string m_Name;
        size_t m_ID = -1;

        Vector3 m_InitialPosition{Vector3(0.0f)};
        Quaternion m_InitialRotation{Quaternion()};
        Vector3 m_InitialScale{Vector3(1.0f)};

        Matrix4x4 m_OffsetMatrix{Matrix4x4::IDENTITY};
        Matrix4x4 m_InverseOffsetMatrix{Matrix4x4::IDENTITY};
        Matrix4x4 m_LocalTransform{Matrix4x4::IDENTITY};
    };

}  // namespace Ethereal
