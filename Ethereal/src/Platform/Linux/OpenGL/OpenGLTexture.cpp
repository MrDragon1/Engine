#include "pch.hpp"
#include "OpenGLTexture.hpp"
#include "Ethereal/Utils/AssetLoader.hpp"
#include <glad/glad.h>

namespace Ethereal
{
    OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
    {
        Ref<TextureData> textureData = CreateRef<TextureData>();
        TextureLoader::LoadPath(path, textureData);
        if(textureData)
            LoadTextureData(textureData);
        
    }

    OpenGLTexture2D::OpenGLTexture2D(const Ref<TextureData>& data) { LoadTextureData(data); }
    

    void OpenGLTexture2D::GetOpenGLTextureFormat(const ETHEREAL_PIXEL_FORMAT& format) {
        // m_InternalFormat: https://www.khronos.org/opengl/wiki/Texture_Storage

        // m_DataFormat: https://docs.gl/gl4/glTexSubImage2D
        // Only: GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_DEPTH_COMPONENT, and GL_STENCIL_INDEX.
        switch (format) {
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R8G8B8_UNORM:
                m_InternalFormat = GL_RGB8;
                m_DataFormat = GL_RGB;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R8G8B8_SRGB:
                m_InternalFormat = GL_SRGB8;
                m_DataFormat = GL_RGB;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R8G8B8A8_UNORM:
                m_InternalFormat = GL_RGBA8;
                m_DataFormat = GL_RGBA;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R8G8B8A8_SRGB:
                m_InternalFormat = GL_SRGB8_ALPHA8;
                m_DataFormat = GL_RGBA;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R32G32_FLOAT:
                m_InternalFormat = GL_RG32F;
                m_DataFormat = GL_RG;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R32G32B32_FLOAT:
                m_InternalFormat = GL_RGB32F;
                m_DataFormat = GL_RGB;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R32G32B32A32_FLOAT:
                m_InternalFormat = GL_RGBA32F;
                m_DataFormat = GL_RGBA;
                break;
            default:
                throw std::runtime_error("invalid texture_byte_size");
                break;
        }
    }

    void OpenGLTexture2D::LoadTextureData(const Ref<TextureData>& data) {
        ET_CORE_ASSERT(data->m_type == ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_2D && data->isValid(), "Invalid image type!");
        GetOpenGLTextureFormat(data->m_format);
        m_IsLoaded = true;

        m_Height = data->m_height;
        m_Width = data->m_width;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data->m_pixels);
    }


    void OpenGLTexture2D::SetData(void* data, uint32_t size) {
        uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;  // bytes per pixel
        ET_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
    }
    
    OpenGLTexture2D::~OpenGLTexture2D() { glDeleteTextures(1, &m_RendererID); }

    void OpenGLTexture2D::Bind(uint32_t slot) const { glBindTextureUnit(slot, m_RendererID); }
}  // namespace Ethereal