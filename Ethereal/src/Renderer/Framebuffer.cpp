#include "pch.h"
#include "Framebuffer.h"

#include "RendererAPI.h"
#include "Platform/Windows/OpenGL/OpenGLFramebuffer.h"

namespace Ethereal
{
    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification &spec)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:    ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return Ref<OpenGLFramebuffer>::Create(spec);
        }
        ET_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}  // namespace Ethereal