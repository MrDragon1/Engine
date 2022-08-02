#include "pch.h"
#include "Texture.h"

#include "RendererAPI.h"
#include "Platform/Windows/OpenGL/OpenGLTexture.h"

namespace Ethereal
{
    Ref<Texture2D> Texture2D::Create(const std::string& path)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:    ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return Ref<OpenGLTexture2D>::Create(path);
        }

        ET_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Texture2D> Texture2D::Create(const Ref<TextureData>& data)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:    ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return Ref<OpenGLTexture2D>::Create(data);
        }

        ET_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
    
    Ref<TextureCube> TextureCube::Create(std::vector<std::string>& paths)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:    ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return Ref<OpenGLTextureCube>::Create(paths);
        }

        ET_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
    
    Ref<TextureCube> TextureCube::Create(const Ref<TextureData>& data)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:    ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return Ref<OpenGLTextureCube>::Create(data);
        }

        ET_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }


}