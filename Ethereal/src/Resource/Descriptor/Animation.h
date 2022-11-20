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
        virtual AssetType GetAssetType() const override { return AssetType::Animation; }

        std::vector<AnimKeyClip> Clips;
        double FramesPersecond;
        TimeStamp Duration;
        std::string Name;

    };

    REFLECTION_TYPE(AnimatorDesc)
    CLASS(AnimatorDesc : public Asset, Fields) {
        REFLECTION_BODY(AnimatorDesc);

      public:
        virtual AssetType GetAssetType() const override { return AssetType::Animator; }

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
        virtual AssetType GetAssetType() const override { return AssetType::Skeleton; }
      public:
        JointDesc RootJoint;
        std::string Name;
    };

}  // namespace Ethereal
