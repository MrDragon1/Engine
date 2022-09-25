#pragma once
#include "pch.h"
#include "Animation.h"
#include "Skeleton.h"
#include "Asset/Asset.h"

namespace Ethereal
{
    // Control how to play animation
    class Animator : public Asset {
      public:
        Ref<Animation> m_Animation;
        Ref<Skeleton> m_Skeleton;
    };

}  // namespace Ethereal
