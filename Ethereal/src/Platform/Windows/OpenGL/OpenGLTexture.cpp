#include "pch.h"
#include "OpenGLTexture.h"
#include "Utils/AssetLoader.h"
#include <glad/glad.h>

namespace Ethereal
{
    namespace Utils
    {
        void GetOpenGLTextureFormat(const ETHEREAL_PIXEL_FORMAT& format, GLenum& internalFormat, GLenum& dataFormat, GLenum& dataType) {
            // m_InternalFormat: https://www.khronos.org/opengl/wiki/Texture_Storage

            // m_DataFormat: https://docs.gl/gl4/glTexSubImage2D
            // Only: GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_DEPTH_COMPONENT, and GL_STENCIL_INDEX.
            switch (format) {
                case ETHEREAL_PIXEL_FORMAT::R8G8B8_UNORM:
                    internalFormat = GL_RGB8;
                    dataFormat = GL_RGB;
                    dataType = GL_UNSIGNED_BYTE;
                    break;
                case ETHEREAL_PIXEL_FORMAT::R8G8B8_SRGB:
                    internalFormat = GL_SRGB8;
                    dataFormat = GL_RGB;
                    dataType = GL_UNSIGNED_BYTE;
                    break;
                case ETHEREAL_PIXEL_FORMAT::R8G8B8A8_UNORM:
                    internalFormat = GL_RGBA8;
                    dataFormat = GL_RGBA;
                    dataType = GL_UNSIGNED_BYTE;
                    break;
                case ETHEREAL_PIXEL_FORMAT::R8G8B8A8_SRGB:
                    internalFormat = GL_SRGB8_ALPHA8;
                    dataFormat = GL_RGBA;
                    dataType = GL_UNSIGNED_BYTE;
                    break;
                case ETHEREAL_PIXEL_FORMAT::R32G32_FLOAT:
                    internalFormat = GL_RG32F;
                    dataFormat = GL_RG;
                    dataType = GL_UNSIGNED_BYTE;
                    break;
                case ETHEREAL_PIXEL_FORMAT::R32G32B32_FLOAT:
                    internalFormat = GL_RGB32F;
                    dataFormat = GL_RGB;
                    dataType = GL_UNSIGNED_BYTE;
                    break;
                case ETHEREAL_PIXEL_FORMAT::R32G32B32A32_FLOAT:
                    internalFormat = GL_RGBA32F;
                    dataFormat = GL_RGBA;
                    dataType = GL_UNSIGNED_BYTE;
                    break;
                case ETHEREAL_PIXEL_FORMAT::R32_INTEGER:
                    internalFormat = GL_R32I;
                    dataFormat = GL_RED_INTEGER;
                    dataType = GL_UNSIGNED_BYTE;
                    break;
                case ETHEREAL_PIXEL_FORMAT::DEPTH:
                    internalFormat = GL_DEPTH_COMPONENT32F;
                    dataFormat = GL_DEPTH_COMPONENT;
                    dataType = GL_FLOAT;
                    break;
                case ETHEREAL_PIXEL_FORMAT::RED:
                    internalFormat = GL_RED;
                    dataFormat = GL_RED;
                    dataType = GL_UNSIGNED_BYTE;
                    break;
                case ETHEREAL_PIXEL_FORMAT::R16G16B16A16_HDR:
                    internalFormat = GL_RGBA16F;  // Convert to RGBA
                    dataFormat = GL_RGB;
                    dataType = GL_FLOAT;
                    break;
                default:
                    ET_CORE_ASSERT("invalid texture type");
                    break;
            }
        }

        void GetOpenGLWarpFormat(const ETHEREAL_WARP_FORMAT& warp, GLenum& WarpFormat) {
            switch (warp) {
                case ETHEREAL_WARP_FORMAT::ETHEREAL_WARP_FORMAT_CLAMP_TO_BORDER:
                    WarpFormat = GL_CLAMP_TO_BORDER;
                    break;
                case ETHEREAL_WARP_FORMAT::ETHEREAL_WARP_FORMAT_CLAMP_TO_EDGE:
                    WarpFormat = GL_CLAMP_TO_EDGE;
                    break;
                case ETHEREAL_WARP_FORMAT::ETHEREAL_WARP_FORMAT_REPEAT:
                    WarpFormat = GL_REPEAT;
                    break;
                default:
                    ET_CORE_ASSERT("invalid texture warp type");
                    break;
            }
        }

        void GetOpenGLFilterFormat(const ETHEREAL_FILTER_FORMAT& filter, GLenum& FilterFormat) {
            switch (filter) {
                case ETHEREAL_FILTER_FORMAT::ETHEREAL_FILTER_FORMAT_NEAREST:
                    FilterFormat = GL_NEAREST;
                    break;
                case ETHEREAL_FILTER_FORMAT::ETHEREAL_FILTER_FORMAT_LINEAR:
                    FilterFormat = GL_LINEAR;
                    break;
                case ETHEREAL_FILTER_FORMAT::ETHEREAL_FILTER_FORMAT_NEAREST_MIPMAP_NEAREST:
                    FilterFormat = GL_NEAREST_MIPMAP_NEAREST;
                    break;
                case ETHEREAL_FILTER_FORMAT::ETHEREAL_FILTER_FORMAT_LINEAR_MIPMAP_NEAREST:
                    FilterFormat = GL_LINEAR_MIPMAP_NEAREST;
                    break;
                case ETHEREAL_FILTER_FORMAT::ETHEREAL_FILTER_FORMAT_NEAREST_MIPMAP_LINEAR:
                    FilterFormat = GL_NEAREST_MIPMAP_LINEAR;
                    break;
                case ETHEREAL_FILTER_FORMAT::ETHEREAL_FILTER_FORMAT_LINEAR_MIPMAP_LINEAR:
                    FilterFormat = GL_LINEAR_MIPMAP_LINEAR;
                    break;
                default:
                    ET_CORE_ASSERT("invalid texture filter type");
                    break;
            }
        }
    }  // namespace Utils

    OpenGLTexture2D::OpenGLTexture2D(const std::string& path) {
        Ref<TextureData> textureData = Ref<TextureData>::Create();
        TextureLoader::LoadPath(path, textureData);
        if (textureData) {
            ET_CORE_ASSERT(textureData->m_type == ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_2D && textureData->isValid(), "Invalid image type!");
            LoadTextureData(textureData);
        }
    }

    OpenGLTexture2D::OpenGLTexture2D(const Ref<TextureData>& data) { LoadTextureData(data); }

    void OpenGLTexture2D::LoadTextureData(const Ref<TextureData>& data) {
        Utils::GetOpenGLTextureFormat(data->m_format, m_InternalFormat, m_DataFormat, m_DataType);
        Utils::GetOpenGLWarpFormat(data->m_warp_format, m_WarpFormat);
        Utils::GetOpenGLFilterFormat(data->m_filter_format, m_FilterFormat);
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

    void OpenGLTexture2D::BindToFramebuffer(uint32_t attachmentid, uint32_t miplevel) const {
        ET_CORE_ASSERT(attachmentid >= 0 && attachmentid <= 3, "Attachment id must be between 0 and 3!");
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentid, m_RendererID, miplevel);
    }
    void OpenGLTexture2D::GenerateMipmaps() const {
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    void OpenGLTexture2D::BindImage(uint32_t slot, uint32_t miplevel) const {
        glBindImageTexture(slot, m_RendererID, miplevel, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
    }

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
    OpenGLTextureCube::OpenGLTextureCube(const Ref<TextureData>& data) {
        // glDeleteTextures(1, &m_RendererID);
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, data->m_width, data->m_height, 0, GL_RGB, GL_FLOAT, nullptr);
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

    void OpenGLTextureCube::BindToFramebuffer(uint32_t attachmentid, uint32_t face, uint32_t miplevel) const {
        ET_CORE_ASSERT(face >= 0 && face <= 5, "Cube map face must be between 0 and 5!");
        ET_CORE_ASSERT(attachmentid >= 0 && attachmentid <= 3, "Attachment id must be between 0 and 3!");
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentid, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_RendererID, miplevel);
    }

    void OpenGLTextureCube::GenerateMipmaps() const {
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    uint32_t OpenGLTextureCube::GetSubTexture(uint32_t layer) const {
        // 实现了从texture array中读取其中的一个slice
        // 但是必须是rgb格式（https://docs.gl/gl4/glCopyImageSubData支持的格式）的texture
        // 单通道的深度图暂时没有实现 (不知道为啥又可以了)
        // TODO: 优化这个函数，不要每次都重新生成一个texture

        uint32_t id;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, nullptr);

        glCopyImageSubData(m_RendererID, GL_TEXTURE_CUBE_MAP, 0, 0, 0, layer,
                            id, GL_TEXTURE_2D, 0, 0, 0, 0,
                            m_Width, m_Height, 1);

        return id;
    }

    OpenGLTexture3D::OpenGLTexture3D(const Ref<TextureData>& data) {
        Utils::GetOpenGLTextureFormat(data->m_format, m_InternalFormat, m_DataFormat, m_DataType);
        Utils::GetOpenGLWarpFormat(data->m_warp_format, m_WarpFormat);
        Utils::GetOpenGLFilterFormat(data->m_filter_format, m_FilterFormat);

        m_IsLoaded = data->isValid();

        m_Height = data->m_height;
        m_Width = data->m_width;
        m_Depth = data->m_depth;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, m_FilterFormat);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, m_FilterFormat);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, m_WarpFormat);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, m_WarpFormat);

        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, m_InternalFormat, m_Width, m_Height, m_Depth, 0, m_DataFormat, m_DataType, nullptr);
    }

    OpenGLTexture3D::~OpenGLTexture3D() { glDeleteTextures(1, &m_RendererID); }

    void OpenGLTexture3D::Bind(uint32_t slot) const { glBindTextureUnit(slot, m_RendererID); }

    void OpenGLTexture3D::BindToFramebufferDepth(uint32_t attachmentid) const {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, attachmentid, 0);
    }

    uint32_t OpenGLTexture3D::GetSubTexture(uint32_t layer) const {
        uint32_t id;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, m_DataType, nullptr);

        glCopyImageSubData(m_RendererID, GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer,
                id, GL_TEXTURE_2D, 0, 0, 0, 0,
                m_Width, m_Height, 1);

//        glCopyImageSubData(textureid, GL_TEXTURE_CUBE_MAP, 0, 0, 0, layer, id, GL_TEXTURE_2D, 0, 0, 0, 0, 32, 32, 1);

//        GLuint fbos[2] = {0, 0};
//        glGenFramebuffers(2, fbos);
//        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbos[0]);
////        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbos[1]);
//
////        GLint drawFboId = 0, readFboId = 0;
////        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);
////        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFboId);
////        ET_CORE_INFO("Draw FBO: {0}, Read FBO: {1}", drawFboId, readFboId );
//
//        glFramebufferTextureLayer(GL_READ_FRAMEBUFFER,
//                GL_DEPTH_ATTACHMENT, m_RendererID, 0, layer);
////        glFramebufferTexture(GL_DRAW_FRAMEBUFFER,
////                GL_DEPTH_ATTACHMENT, id, 0);
//
////        const GLenum err = glGetError();
////        if (GL_NO_ERROR == err) {
////            ET_CORE_INFO("No error");
////        } else {
////            ET_CORE_INFO("Error: {0}", err);
////        }
//
//        auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//        if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
//            std::cout << "Framebuffer not complete: " << fboStatus << std::endl;
//
////        glBlitFramebuffer(
////                0, 0, m_Width, m_Height, 0, 0, m_Width, m_Height,
////                GL_DEPTH_BUFFER_BIT , GL_NEAREST);
//
//        glDeleteFramebuffers(2, fbos);

        return id;
    }

}  // namespace Ethereal