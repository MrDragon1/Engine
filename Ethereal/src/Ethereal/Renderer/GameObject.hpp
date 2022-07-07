#pragma once
#include "pch.hpp"
#include "Common.hpp"

namespace Ethereal
{
    class GameObject {
      public:
        GameObject() = default;
        void AddRenderEntity(RenderEntity renderEntity);

        std::vector<RenderEntity> m_RenderEntities;
    };
}  // namespace Ethereal