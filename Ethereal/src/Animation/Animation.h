#pragma once
#include "pch.h"
#include "Asset/Asset.h"
namespace Ethereal
{
    struct AnimState {
        glm::vec3 Position;
        glm::quat Rotation;
        glm::vec3 Scale;
        float TimeStamp;
    };

    struct AnimKeyClip {
        std::vector<AnimState> States;
        size_t JointID;
    };

    class Animation : public Asset {
      public:
        std::vector<AnimKeyClip> m_KeyClips;
        size_t m_TotalFrames;
        std::string m_Name{"Default Animation"};
    };

}  // namespace Ethereal
