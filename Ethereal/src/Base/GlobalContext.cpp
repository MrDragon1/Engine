#include "GlobalContext.h"

namespace Ethereal {
RenderSystem GlobalContext::m_RenderSystem;

Vector2 GlobalContext::m_ViewportSize = {1280.0f, 720.0f};
Backend::Driver* GlobalContext::mDriver = nullptr;
Ref<UniformManager> GlobalContext::mUniformManager = nullptr;

GlobalContext::GlobalContext() {}

void GlobalContext::Init() {
    mDriver = new Backend::Driver(BackendType::OPENGL);
    mUniformManager = Ref<UniformManager>::Create(GetDriverApi());
}

void GlobalContext::Reset() {
    Ref<Project> project = Ref<Project>::Create();
    Project::SetActive(project);  // Set nullptr when Application deconstruct

    m_RenderSystem.Init();
}
}  // namespace Ethereal