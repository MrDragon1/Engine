#pragma once

#include "Ethereal/Renderer/Texture.hpp"

namespace Ethereal
{
    class OpenGLTexture2D : public Texture2D {
      public:
        OpenGLTexture2D(const std::string& path);
        OpenGLTexture2D(const Ref<TextureData>& data);
        virtual ~OpenGLTexture2D();

        virtual uint32_t GetWidth() const override { return m_Width; };
        virtual uint32_t GetHeight() const override { return m_Height; };
        virtual uint32_t GetRendererID() const override { return m_RendererID; };

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void SetData(void* data, uint32_t size) override;
        virtual void Clear(int data) override;
        virtual bool operator==(const Texture& other) const override { return m_RendererID == other.GetRendererID(); }
        virtual bool IsLoaded() const override { return m_IsLoaded; };

      private:
        void GetOpenGLTextureFormat(const ETHEREAL_PIXEL_FORMAT& format);
        void GetOpenGLWarpFormat(const ETHEREAL_WARP_FORMAT& warp);
        void GetOpenGLFilterFormat(const ETHEREAL_FILTER_FORMAT& filter);

        void LoadTextureData(const Ref<TextureData>& data);
        uint32_t m_RendererID;
        uint32_t m_Width, m_Height;
        GLenum m_InternalFormat, m_DataFormat;
        GLenum m_DataType;
        GLenum m_WarpFormat, m_FilterFormat;
        bool m_IsLoaded = false;
    };

    class OpenGLTextureCube : public TextureCube {
      public:
        OpenGLTextureCube(std::vector<std::string>& paths);
        OpenGLTextureCube(const Ref<TextureData>& data);
        virtual ~OpenGLTextureCube();

        virtual uint32_t GetWidth() const override { return m_Width; };
        virtual uint32_t GetHeight() const override { return m_Height; };
        virtual uint32_t GetRendererID() const override { return m_RendererID; };

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void SetData(void* data, uint32_t size) override { ET_CORE_WARN("Texture Cube does not support SetData"); };
        virtual void Clear(int data) override { ET_CORE_WARN("Texture Cube does not support Clear data"); };
        virtual bool operator==(const Texture& other) const override { return m_RendererID == other.GetRendererID(); }
        virtual bool IsLoaded() const override { return m_IsLoaded; };

        virtual void BindToFramebuffer(uint32_t face) const override;
      private:
        void LoadTextureData(const Ref<TextureData>& data);
        uint32_t m_RendererID;
        uint32_t m_Width, m_Height;

        bool m_IsLoaded = false;
    };
}  // namespace Ethereal
