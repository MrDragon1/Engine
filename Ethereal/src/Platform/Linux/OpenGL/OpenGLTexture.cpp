#include "pch.h"
#include "OpenGLTexture.h"
#include "Utils/AssetLoader.h"
#include <glad/glad.h>

namespace Ethereal
{
    OpenGLTexture2D::OpenGLTexture2D(const std::string& path) {
        Ref<TextureData> textureData = Ref<TextureData>::Create();
        TextureLoader::LoadPath(path, textureData);
        if (textureData) {
            ET_CORE_ASSERT(textureData->m_type == ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_2D && textureData->isValid(), "Invalid image type!");
            LoadTextureData(textureData);
        }
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
                m_DataType = GL_UNSIGNED_BYTE;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R8G8B8_SRGB:
                m_InternalFormat = GL_SRGB8;
                m_DataFormat = GL_RGB;
                m_DataType = GL_UNSIGNED_BYTE;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R8G8B8A8_UNORM:
                m_InternalFormat = GL_RGBA8;
                m_DataFormat = GL_RGBA;
                m_DataType = GL_UNSIGNED_BYTE;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R8G8B8A8_SRGB:
                m_InternalFormat = GL_SRGB8_ALPHA8;
                m_DataFormat = GL_RGBA;
                m_DataType = GL_UNSIGNED_BYTE;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R32G32_FLOAT:
                m_InternalFormat = GL_RG32F;
                m_DataFormat = GL_RG;
                m_DataType = GL_UNSIGNED_BYTE;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R32G32B32_FLOAT:
                m_InternalFormat = GL_RGB32F;
                m_DataFormat = GL_RGB;
                m_DataType = GL_UNSIGNED_BYTE;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R32G32B32A32_FLOAT:
                m_InternalFormat = GL_RGBA32F;
                m_DataFormat = GL_RGBA;
                m_DataType = GL_UNSIGNED_BYTE;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R32_INTEGER:
                m_InternalFormat = GL_R32I;
                m_DataFormat = GL_RED_INTEGER;
                m_DataType = GL_UNSIGNED_BYTE;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_DEPTH:
                m_InternalFormat = GL_DEPTH_COMPONENT;
                m_DataFormat = GL_DEPTH_COMPONENT;
                m_DataType = GL_FLOAT;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_RED:
                m_InternalFormat = GL_RED;
                m_DataFormat = GL_RED;
                m_DataType = GL_UNSIGNED_BYTE;
                break;
            case ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_HDR:
                m_InternalFormat = GL_RGB16F;
                m_DataFormat = GL_RGB;
                m_DataType = GL_FLOAT;
                break;
            default:
                ET_CORE_ASSERT("invalid texture type");
                break;
        }
    }

    void OpenGLTexture2D::GetOpenGLWarpFormat(const ETHEREAL_WARP_FORMAT& warp) {
        switch (warp) {
            case ETHEREAL_WARP_FORMAT::ETHEREAL_WARP_FORMAT_CLAMP_TO_BORDER:
                m_WarpFormat = GL_CLAMP_TO_BORDER;
                break;
            case ETHEREAL_WARP_FORMAT::ETHEREAL_WARP_FORMAT_CLAMP_TO_EDGE:
                m_WarpFormat = GL_CLAMP_TO_EDGE;
                break;
            case ETHEREAL_WARP_FORMAT::ETHEREAL_WARP_FORMAT_REPEAT:
                m_WarpFormat = GL_REPEAT;
                break;
            default:
                ET_CORE_ASSERT("invalid texture warp type");
                break;
        }
    }

    void OpenGLTexture2D::GetOpenGLFilterFormat(const ETHEREAL_FILTER_FORMAT& filter) {
        switch (filter) {
            case ETHEREAL_FILTER_FORMAT::ETHEREAL_FILTER_FORMAT_NEAREST:
                m_FilterFormat = GL_NEAREST;
                break;
            case ETHEREAL_FILTER_FORMAT::ETHEREAL_FILTER_FORMAT_LINEAR:
                m_FilterFormat = GL_LINEAR;
                break;
            case ETHEREAL_FILTER_FORMAT::ETHEREAL_FILTER_FORMAT_NEAREST_MIPMAP_NEAREST:
                m_FilterFormat = GL_NEAREST_MIPMAP_NEAREST;
                break;
            case ETHEREAL_FILTER_FORMAT::ETHEREAL_FILTER_FORMAT_LINEAR_MIPMAP_NEAREST:
                m_FilterFormat = GL_LINEAR_MIPMAP_NEAREST;
                break;
            case ETHEREAL_FILTER_FORMAT::ETHEREAL_FILTER_FORMAT_NEAREST_MIPMAP_LINEAR:
                m_FilterFormat = GL_NEAREST_MIPMAP_LINEAR;
                break;
            case ETHEREAL_FILTER_FORMAT::ETHEREAL_FILTER_FORMAT_LINEAR_MIPMAP_LINEAR:
                m_FilterFormat = GL_LINEAR_MIPMAP_LINEAR;
                break;
            default:
                ET_CORE_ASSERT("invalid texture filter type");
                break;
        }
    }

    void OpenGLTexture2D::LoadTextureData(const Ref<TextureData>& data) {
        GetOpenGLTextureFormat(data->m_format);
        GetOpenGLWarpFormat(data->m_warp_format);
        GetOpenGLFilterFormat(data->m_filter_format);
        m_IsLoaded = data->isValid();

        m_Height = data->m_height;
        m_Width = data->m_width;
        // glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        // glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_FilterFormat);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_FilterFormat);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_WarpFormat);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_WarpFormat);

        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, m_DataType, m_IsLoaded ? data->m_pixels : nullptr);
        // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, dataType, m_IsLoaded ? data->m_pixels : nullptr);
    }

    void OpenGLTexture2D::SetData(void* data, uint32_t size) {
        uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;  // bytes per pixel
        ET_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);
        // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
    }

    void OpenGLTexture2D::Clear(int value) { glClearTexImage(m_RendererID, 0, m_DataFormat, GL_INT, &value); }

    OpenGLTexture2D::~OpenGLTexture2D() { glDeleteTextures(1, &m_RendererID); }

    void OpenGLTexture2D::Bind(uint32_t slot) const { glBindTextureUnit(slot, m_RendererID); }

    // for non-hdr cube map only
    OpenGLTextureCube::OpenGLTextureCube(std::vector<std::string>& paths) {
        glDeleteTextures(1, &m_RendererID);
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < paths.size(); i++) {
            Ref<TextureData> textureData = Ref<TextureData>::Create();
            TextureLoader::LoadPath(paths[i], textureData);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, textureData->m_width, textureData->m_height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         textureData->m_pixels);
            m_Width = textureData->m_width;
            m_Height = textureData->m_height;
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    
    // For hdr map only
    OpenGLTextureCube::OpenGLTextureCube(const Ref<TextureData>& data)
    {
        //glDeleteTextures(1, &m_RendererID);
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, data->m_width, data->m_height, 0, GL_RGB, GL_FLOAT,
                         nullptr);
            m_Width = data->m_width;
            m_Height = data->m_height;
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    OpenGLTextureCube::~OpenGLTextureCube() { glDeleteTextures(1, &m_RendererID); }

    void OpenGLTextureCube::Bind(uint32_t slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
    }

    void OpenGLTextureCube::BindToFramebuffer(uint32_t attachmentid, uint32_t face, uint32_t miplevel) const
    {
        ET_CORE_ASSERT(face >= 0 && face <= 5, "Cube map face must be between 0 and 5!");
        ET_CORE_ASSERT(attachmentid >= 0 && attachmentid <= 3, "Attachment id must be between 0 and 3!");
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentid, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_RendererID, miplevel);
    }

    void OpenGLTextureCube::GenerateMipmaps() const
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

}  // namespace Ethereal