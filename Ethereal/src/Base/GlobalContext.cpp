#include "GlobalContext.h"

namespace Ethereal
{
    RenderSystem GlobalContext::m_RenderSystem;
    glm::vec2 GlobalContext::m_ViewportSize = {1280.0f, 720.0f};
    GlobalContext::GlobalContext() {
    }

    void GlobalContext::Reset() {
        Ref<Project> project = Ref<Project>::Create();
        Project::SetActive(project);  // Set nullptr when Application deconstruct

        m_RenderSystem.Init();
    }
}