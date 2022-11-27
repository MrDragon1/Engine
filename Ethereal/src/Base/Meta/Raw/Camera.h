#pragma once
#include "Base/Meta/Reflection.h"

namespace Ethereal
{
    REFLECTION_TYPE(CameraRaw)
    CLASS(CameraRaw, Fields) {
        REFLECTION_BODY(CameraRaw);
      public:
        bool Primary{false};
        bool FixedAspectRatio{false};
        bool Perspective{true};
        float Fov{45.0f};
        float Near{0.1f};
        float Far{1000.0f};
        float AspectRatio{1.778f};
        float OrthographicSize{10.0f};
        float OrthographicNear{0.1f};
        float OrthographicFar{1000.0f};
    };
}
