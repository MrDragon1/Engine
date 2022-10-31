#include "Animation.h"

namespace Ethereal
{
    Animation::Animation(const Ref<Animation>& anim) {
        m_KeyClips = anim->m_KeyClips;
        m_Name = anim->m_Name;
        m_FramesPersecond = anim->m_FramesPersecond;
        m_Duration = anim->m_Duration;
    }

    AnimInterClip Animation::GetInterpolateClip(TimeStamp animationTime) {
        AnimInterClip clips;
        std::vector<int> positionIndex = GetPositionIndex(animationTime);
        std::vector<int> rotationIndex = GetRotationIndex(animationTime);
        std::vector<int> scaleIndex = GetScaleIndex(animationTime);

        for (int i = 0; i < m_KeyClips.size(); i++) {
            AnimState state{};

            state.JointID = m_KeyClips[i].JointID;
            state.PositionState = InterpolatePosition(i, positionIndex[i], animationTime);
            state.RotationState = InterpolateRotation(i, positionIndex[i], animationTime);
            state.ScaleState = InterpolateScale(i, positionIndex[i], animationTime);

            clips.States.push_back(state);
        }
        return clips;
    }

    std::vector<int> Animation::GetPositionIndex(TimeStamp animationTime) {
        std::vector<int> res;
        res.clear();
        for (auto& joint : m_KeyClips) {
            for (int index = 0; index < joint.PositionStates.size() - 1; ++index) {
                if (animationTime < joint.PositionStates[index + 1].TimeStamp) res.push_back(index);
            }
        }
        return res;
    }

    std::vector<int> Animation::GetRotationIndex(TimeStamp animationTime) {
        std::vector<int> res;
        res.clear();
        for (auto& joint : m_KeyClips) {
            for (int index = 0; index < joint.RotationStates.size() - 1; ++index) {
                if (animationTime < joint.RotationStates[index + 1].TimeStamp) res.push_back(index);
            }
        }
        return res;
    }

    std::vector<int> Animation::GetScaleIndex(TimeStamp animationTime) {
        std::vector<int> res;
        res.clear();
        for (auto& joint : m_KeyClips) {
            for (int index = 0; index < joint.ScaleStates.size() - 1; ++index) {
                if (animationTime < joint.ScaleStates[index + 1].TimeStamp) res.push_back(index);
            }
        }
        return res;
    }

    float Animation::GetScaleFactor(TimeStamp lastTimeStamp, TimeStamp nextTimeStamp, TimeStamp animationTime) {
        float scaleFactor = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff = nextTimeStamp - lastTimeStamp;
        scaleFactor = midWayLength / framesDiff;
        return scaleFactor;
    }

    AnimPositionState Animation::InterpolatePosition(int jointIndex, int keyIndex, TimeStamp animationTime) {
        auto& joint = m_KeyClips[jointIndex];
        AnimPositionState res;
        res.TimeStamp = animationTime;
        if (1 == joint.PositionStates.size()) {
            res.Position = joint.PositionStates[0].Position;
            return res;
        }

        int p0Index = keyIndex;
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(joint.PositionStates[p0Index].TimeStamp, joint.PositionStates[p1Index].TimeStamp, animationTime);
        res.Position = glm::mix(joint.PositionStates[p0Index].Position, joint.PositionStates[p1Index].Position, scaleFactor);
        return res;
    }

    AnimRotationState Animation::InterpolateRotation(int jointIndex, int keyIndex, TimeStamp animationTime) {
        auto& joint = m_KeyClips[jointIndex];

        AnimRotationState res;
        res.TimeStamp = animationTime;

        if (1 == joint.RotationStates.size()) {
            res.Rotation = joint.RotationStates[0].Rotation;
            return res;
        }

        int p0Index = keyIndex;
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(joint.RotationStates[p0Index].TimeStamp, joint.RotationStates[p1Index].TimeStamp, animationTime);
        res.Rotation = glm::mix(joint.RotationStates[p0Index].Rotation, joint.RotationStates[p1Index].Rotation, scaleFactor);
        return res;
    }

    AnimScaleState Animation::InterpolateScale(int jointIndex, int keyIndex, TimeStamp animationTime) {
        auto& joint = m_KeyClips[jointIndex];

        AnimScaleState res;
        res.TimeStamp = animationTime;

        if (1 == joint.ScaleStates.size()) {
            res.Scale = joint.ScaleStates[0].Scale;
            return res;
        }

        int p0Index = keyIndex;
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(joint.ScaleStates[p0Index].TimeStamp, joint.ScaleStates[p1Index].TimeStamp, animationTime);
        res.Scale = glm::mix(joint.ScaleStates[p0Index].Scale, joint.ScaleStates[p1Index].Scale, scaleFactor);
        return res;
    }

}  // namespace Ethereal