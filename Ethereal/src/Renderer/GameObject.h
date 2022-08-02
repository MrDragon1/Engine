#pragma once
//#include "pch.h"
#include "Common.h"

namespace Ethereal
{
    class GameObject : public RefCounted{
      public:
        GameObject() = default;
        void AddRenderEntity(RenderEntity renderEntity);

        std::vector<RenderEntity> m_RenderEntities;//One object has many sub objects
    };
}  // namespace Ethereal