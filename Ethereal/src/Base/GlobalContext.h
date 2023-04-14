#pragma once

#include "Base/Math/Vector.h"
#include "Base/Singleton.h"
#include "Core/Project/Project.h"
#include "Core/Renderer/RenderSystem.h"

#include "Backend/Driver.h";
#include "Core/Renderer/Uniform/UniformManager.h"
namespace Ethereal {
class GlobalContext final : public Singleton<GlobalContext> {
   public:
    GlobalContext();
    GlobalContext(const GlobalContext&) = delete;
    GlobalContext& operator=(const GlobalContext&) = delete;

   public:
    static void Init();
    static void Reset();
    static RenderSystem& GetRenderSystem() { return m_RenderSystem; };
    static Vector2& GetViewportSize() { return m_ViewportSize; };
    static void SetViewportSize(const Vector2& viewportSize) { m_ViewportSize = viewportSize; };
    static Ref<Backend::DriverApi> GetDriverApi() {
        ET_CORE_ASSERT(mDriver, "Driver is not initialized!");
        return mDriver->GetApi();
    }

    static Ref<UniformManager> GetUniformManager() { return mUniformManager; }

   private:
    static RenderSystem m_RenderSystem;
    static Backend::Driver* mDriver;
    static Ref<UniformManager> mUniformManager;
    static Vector2 m_ViewportSize;
};
}  // namespace Ethereal