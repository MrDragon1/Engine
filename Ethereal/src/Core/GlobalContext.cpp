#include "GlobalContext.h"
// #include "Project/Project.h"

namespace Ethereal
{
    RenderSystem GlobalContext::m_RenderSystem;
    glm::vec2 GlobalContext::m_ViewportSize = {1280.0f, 720.0f};
    GlobalContext::GlobalContext() {
        //        Ref<Project> project = Ref<Project>::Create();
        //        Project::SetActive(project);
    }

    void GlobalContext::Reset()
    {
        m_RenderSystem.Init();
    }
}