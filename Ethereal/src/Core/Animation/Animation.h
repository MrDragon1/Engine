#pragma once
#include "pch.h"
#include "Core/Asset/Asset.h"
namespace Ethereal
{
    struct AnimPositionState {
        glm::vec3 Position;
        TimeStamp TimeStamp;
    };
    struct AnimRotationState {
        glm::quat Rotation;
        TimeStamp TimeStamp;
    };
    struct AnimScaleState {
        glm::vec3 Scale;
        TimeStamp TimeStamp;
    };

    struct AnimKeyClip {
        std::vector<AnimPositionState> PositionStates;
        std::vector<AnimRotationState> RotationStates;
        std::vector<AnimScaleState> ScaleStates;
        size_t JointID;
    };

    struct AnimState {
        AnimPositionState PositionState;
        AnimRotationState RotationState;
        AnimScaleState ScaleState;
        size_t JointID;
    };

    struct AnimInterClip {
        std::vector<AnimState> States;
    };

    class Animation : public Asset {
      public:
        Animation() = default;
        Animation(const Ref<Animation>& anim);
        AnimInterClip GetInterpolateClip(TimeStamp animationTime);
        double GetFramesPerSecond() const { return m_FramesPersecond; }
        TimeStamp GetDuration() const { return m_Duration; }

        static AssetType GetStaticType() { return AssetType::Animation; }
        AssetType GetAssetType() const override { return GetStaticType(); }

      public:
        std::vector<AnimKeyClip> m_KeyClips;
        double m_FramesPersecond;  // Frame per second
        TimeStamp m_Duration;      // Duration (total frames)
        std::string m_Name{"Default Animation"};

      private:
        std::vector<int> GetPositionIndex(TimeStamp animationTime);
        std::vector<int> GetRotationIndex(TimeStamp animationTime);
        std::vector<int> GetScaleIndex(TimeStamp animationTime);
        float GetScaleFactor(TimeStamp lastTimeStamp, TimeStamp nextTimeStamp, TimeStamp animationTime);

        AnimPositionState InterpolatePosition(int jointIndex, int keyIndex, TimeStamp animationTime);
        AnimRotationState InterpolateRotation(int jointIndex, int keyIndex, TimeStamp animationTime);
        AnimScaleState InterpolateScale(int jointIndex, int keyIndex, TimeStamp animationTime);
    };

}  // namespace Ethereal
