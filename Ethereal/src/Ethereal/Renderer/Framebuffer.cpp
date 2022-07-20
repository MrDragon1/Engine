#include "pch.hpp"
#include "Framebuffer.hpp"

#include "Ethereal/Renderer/Renderer.hpp"
#include "Platform/Linux/OpenGL/OpenGLFramebuffer.hpp"

namespace Ethereal
{
    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification &spec)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:    ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return CreateRef<OpenGLFramebuffer>(spec);
        }
        ET_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}  // namespace Ethereal