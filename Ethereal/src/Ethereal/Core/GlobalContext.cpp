#include "GlobalContext.hpp"

namespace Ethereal
{
    RenderSystem GlobalContext::m_RenderSystem;

    GlobalContext::GlobalContext()
    {
    }
    
    void GlobalContext::Reset()
    {
        m_RenderSystem.Init();
    }
}