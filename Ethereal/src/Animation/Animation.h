#pragma once
#include "pch.h"
#include "Asset/Asset.h"
namespace Ethereal
{
    struct AnimState {
        glm::vec3 Position;
        glm::quat Rotation;
        glm::vec3 Scale;
        TimeStamp TimeStamp;  // Frame Number
    };

    struct AnimKeyClip {
        std::vector<AnimState> States;
        size_t JointID;
    };

    struct AnimInterClip {
        std::vector<AnimKeyClip> States;
    };

    class Animation : public Asset {
      public:
        AnimInterClip GetInterpolateClip(TimeStamp ts);

        size_t GetFramesPerSecond() { return m_TotalFrames; }
        TimeStamp GetDuration() { return m_Duration; }

      public:
        std::vector<AnimKeyClip> m_KeyClips;
        size_t m_TotalFrames;
        TimeStamp m_Duration;
        std::string m_Name{"Default Animation"};

      private:
        AnimState InterpolateState(AnimState prev, AnimState next, TimeStamp ts);
    };

}  // namespace Ethereal
