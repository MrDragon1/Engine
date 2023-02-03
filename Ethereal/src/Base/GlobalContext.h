#pragma once

#include "Base/Singleton.h"
#include "Base/Math/Vector.h"
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
        static void Init();
        static void Reset();
        static RenderSystem& GetRenderSystem() { return m_RenderSystem; };
        static ShaderLibrary& GetShaderLibrary() { return m_ShaderLibrary; };
        static Vector2& GetViewportSize() { return m_ViewportSize; };
        static void SetViewportSize(const Vector2& viewportSize) { m_ViewportSize = viewportSize; };

      private:
        static RenderSystem m_RenderSystem;
        static ShaderLibrary m_ShaderLibrary;
        static Vector2 m_ViewportSize;
    };
}  // namespace Ethereal