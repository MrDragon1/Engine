#pragma once

#include "Ethereal/Core/Base/Singleton.hpp"
#include "Ethereal/Renderer/RenderSystem.hpp"

namespace Ethereal
{
    class GlobalContext final : public Singleton<GlobalContext> {
      public:
        GlobalContext();
        GlobalContext(const GlobalContext&) = delete;
        GlobalContext& operator=(const GlobalContext&) = delete;

      public:
        static void Reset();
        static RenderSystem& GetRenderSystem() { return m_RenderSystem; };

      private:
        static RenderSystem m_RenderSystem;
    };
}  // namespace Ethereal