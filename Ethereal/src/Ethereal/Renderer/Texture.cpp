#include "pch.hpp"
#include "Texture.hpp"

#include "Renderer.hpp"
#include "Platform/Linux/OpenGL/OpenGLTexture.hpp"

namespace Ethereal
{
    Ref<Texture2D> Texture2D::Create(const std::string& path)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:    ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTexture2D>(path);
        }

        ET_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Texture2D> Texture2D::Create(const Ref<TextureData>& data)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:    ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTexture2D>(data);
        }

        ET_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}