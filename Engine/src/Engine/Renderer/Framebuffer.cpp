#include "pch.hpp"
#include "Framebuffer.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Platform/Linux/OpenGL/OpenGLFramebuffer.hpp"

namespace Engine
{
    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification &spec)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:    ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return CreateRef<OpenGLFramebuffer>(spec);
        }
        ENGINE_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}  // namespace Engine