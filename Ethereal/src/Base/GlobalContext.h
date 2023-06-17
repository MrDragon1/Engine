#pragma once

#include "Base/Math/Vector.h"
#include "Base/Singleton.h"
#include "Core/Project/Project.h"
#include "Core/Renderer/RenderSystem.h"

#include "Backend/Driver.h";
#include "Core/Renderer/Uniform/UniformManager.h"
#include "Core/Renderer/LightManager.h"
namespace Ethereal {
class GlobalContext final : public Singleton<GlobalContext> {
   public:
    GlobalContext();
    GlobalContext(const GlobalContext&) = delete;
    GlobalContext& operator=(const GlobalContext&) = delete;

   public:
    static void Init();
    static void Reset();
    static RenderSystem& GetRenderSystem() { return mRenderSystem; };
    static LightManager& GetLightManager() { return mLightManager; };
    static Vector2& GetViewportSize() { return mViewportSize; };
    static void SetViewportSize(const Vector2& viewportSize) { mViewportSize = viewportSize; };
    static Ref<Backend::DriverApi> GetDriverApi() {
        ET_CORE_ASSERT(mDriver, "Driver is not initialized!");
        return mDriver->GetApi();
    }

    static Ref<UniformManager> GetUniformManager() { return mUniformManager; }
    static BackendType GetBackendType() { return mBackendType; }

   private:
    static RenderSystem mRenderSystem;
    static Backend::Driver* mDriver;
    static Ref<UniformManager> mUniformManager;
    static LightManager mLightManager;
    static Vector2 mViewportSize;
    static BackendType mBackendType;
};
}  // namespace Ethereal