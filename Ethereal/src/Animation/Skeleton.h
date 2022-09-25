#pragma once
#include "Joint.h"

namespace Ethereal
{
    // The skeleton that describes how the joints connect
    class Skeleton : public RefCounted {
      public:
        Joint m_Root;
        std::vector<Ref<Joint>> m_Joints;
    };

}  // namespace Ethereal
