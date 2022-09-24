#pragma once
#include "pch.h"
#include "Asset/Asset.h"
namespace Ethereal
{
    struct AnimKeyClip {
        glm::vec3 Position;
        glm::quat Rotation;
        glm::vec3 Scale;
        float TimeStamp;
    };

    class Animation : public Asset {
      public:
        std::vector<AnimKeyClip> m_KeyClips;
        size_t m_TotalFrames;
    };

}  // namespace Ethereal
