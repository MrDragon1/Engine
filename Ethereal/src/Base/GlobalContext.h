#pragma once

#include "Base/Singleton.h"
#include "Core/Renderer/RenderSystem.h"
#include "Core/Project/Project.h"

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
        static glm::vec2& GetViewportSize() { return m_ViewportSize; };
        static void SetViewportSize(const glm::vec2& viewportSize) { m_ViewportSize = viewportSize; };

      private:
        static RenderSystem m_RenderSystem;
        static glm::vec2 m_ViewportSize;
    };
}  // namespace Ethereal