#pragma once
#include "Core/Asset/Asset.h"

namespace Ethereal
{
    REFLECTION_TYPE(AnimPositionState)
    CLASS(AnimPositionState, Fields) {
        REFLECTION_BODY(AnimPositionState);

      public:
        Vector3 Position;
        TimeStamp TimeStamp;
    };

    REFLECTION_TYPE(AnimRotationState)
    CLASS(AnimRotationState, Fields) {
        REFLECTION_BODY(AnimRotationState);

      public:
        Quaternion Rotation;
        TimeStamp TimeStamp;
    };

    REFLECTION_TYPE(AnimScaleState)
    CLASS(AnimScaleState, Fields) {
        REFLECTION_BODY(AnimScaleState);

      public:
        Vector3 Scale;
        TimeStamp TimeStamp;
    };

    REFLECTION_TYPE(AnimKeyClip)
    CLASS(AnimKeyClip, Fields) {
        REFLECTION_BODY(AnimKeyClip);

      public:
        std::vector<AnimPositionState> PositionStates;
        std::vector<AnimRotationState> RotationStates;
        std::vector<AnimScaleState> ScaleStates;
        size_t JointID;
    };

    REFLECTION_TYPE(AnimState)
    CLASS(AnimState, Fields) {
        REFLECTION_BODY(AnimState);

      public:
        AnimPositionState PositionState;
        AnimRotationState RotationState;
        AnimScaleState ScaleState;
        size_t JointID;
    };

    REFLECTION_TYPE(AnimationDesc)
    CLASS(AnimationDesc : public Asset, Fields) {
        REFLECTION_BODY(AnimationDesc);

      public:
        AnimationDesc() = default;
        AnimationDesc(const AnimationDesc& other) {
            this->Name = other.Name;
            this->Duration = other.Duration;
            this->FramesPersecond = other.FramesPersecond;
            this->Clips = other.Clips;
        }
        static AssetType GetStaticType() { return AssetType::Animation; }
        virtual AssetType GetAssetType() const override { return GetStaticType(); }
        std::vector<AnimKeyClip> Clips;
        double FramesPersecond;
        TimeStamp Duration;
        std::string Name;

    };

    REFLECTION_TYPE(AnimatorDesc)
    CLASS(AnimatorDesc : public Asset, Fields) {
        REFLECTION_BODY(AnimatorDesc);

      public:
        AnimatorDesc() = default;
        AnimatorDesc(const AnimatorDesc& other) {
            this->Animation = other.Animation;
            this->Skeleton = other.Skeleton;
        }

        static AssetType GetStaticType() { return AssetType::Animator; }
        virtual AssetType GetAssetType() const override { return GetStaticType(); }
        UUID Animation;
        UUID Skeleton;
    };


    REFLECTION_TYPE(JointDesc)
    CLASS(JointDesc, Fields) {
        REFLECTION_BODY(JointDesc);

      public:
        std::vector<JointDesc> Children;
        std::string Name;
        size_t ID = -1;

        Vector3 InitialPosition;
        Quaternion InitialRotation;
        Vector3 InitialScale;

        Matrix4 OffsetMatrix;
        Matrix4 InverseOffsetMatrix;
        Matrix4 LocalTransform;
    };


    REFLECTION_TYPE(SkeletonDesc)
    CLASS(SkeletonDesc : public Asset, Fields) {
        REFLECTION_BODY(SkeletonDesc);

      public:
        SkeletonDesc() = default;
        SkeletonDesc(const SkeletonDesc& other) {
            this->RootJoint = other.RootJoint;
            this->Name = other.Name;

        }
        static AssetType GetStaticType() { return AssetType::Skeleton; }
        virtual AssetType GetAssetType() const override { return GetStaticType(); }
      public:
        JointDesc RootJoint;
        std::string Name;
    };

}  // namespace Ethereal
