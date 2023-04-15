#pragma once
#include "pch.h"
#include "Core/Animation/Animation.h"
#include "Resource/Descriptor/Animation.h"
namespace Ethereal
{
    // The Joint infomation, not include any animation info
    class Joint : public RefCounted {
      public:
        Joint() = default;

        void Load(const JointDesc& desc);
        void Save(JointDesc& desc);

        void SetLocalTransform(AnimState state);

        Matrix4 GetLocalTransform() { return mLocalTransform; }

      public:
        Ref<Joint> mParent = nullptr;
        std::vector<Ref<Joint>> mChildren;
        std::string mName;
        size_t mID = -1;

        Vector3 mInitialPosition{Vector3(0.0f)};
        Quaternion mInitialRotation{Quaternion()};
        Vector3 mInitialScale{Vector3(1.0f)};

        Matrix4 mOffsetMatrix{Matrix4::IDENTITY};
        Matrix4 mInverseOffsetMatrix{Matrix4::IDENTITY};
        Matrix4 mLocalTransform{Matrix4::IDENTITY};
    };

}  // namespace Ethereal
