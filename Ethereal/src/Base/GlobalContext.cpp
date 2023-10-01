#include "GlobalContext.h"

namespace Ethereal {
RenderSystem GlobalContext::mRenderSystem;
LightManager GlobalContext::mLightManager;
Property GlobalContext::mProperty;

Vector2 GlobalContext::mViewportSize = {1280.0f, 720.0f};
Backend::Driver* GlobalContext::mDriver = nullptr;
Ref<UniformManager> GlobalContext::mUniformManager = nullptr;
BackendType GlobalContext::mBackendType = BackendType::VULKAN;

GlobalContext::GlobalContext() {}

void GlobalContext::Init() {
    mDriver = new Backend::Driver(mBackendType);
    mUniformManager = Ref<UniformManager>::Create(GetDriverApi());
}

void GlobalContext::Reset() {
    Ref<Project> project = Ref<Project>::Create();
    Project::SetActive(project);  // Set nullptr when Application deconstruct

    mRenderSystem.Init();
}
}  // namespace Ethereal