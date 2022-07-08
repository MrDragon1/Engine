#pragma once

#include "Ethereal/Renderer/Texture.hpp"

namespace Ethereal
{
    class OpenGLTexture2D : public Texture2D {
      public:
        OpenGLTexture2D(const std::string& path);
        OpenGLTexture2D(const Ref<TextureData>& path);
        virtual ~OpenGLTexture2D();

        virtual uint32_t GetWidth() const override { return m_Width; };
        virtual uint32_t GetHeight() const override { return m_Height; };
        virtual uint32_t GetRendererID() const override { return m_RendererID; };

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void SetData(void* data, uint32_t size) override;
        virtual bool operator==(const Texture& other) const override {
            return m_RendererID == other.GetRendererID();
        }
        virtual bool IsLoaded() const override { return m_IsLoaded;};
      private:
        void GetOpenGLTextureFormat(const ETHEREAL_PIXEL_FORMAT& format);
        void LoadTextureData(const Ref<TextureData>& data);
        uint32_t m_RendererID;
        uint32_t m_Width, m_Height;
        GLenum m_InternalFormat, m_DataFormat;
        bool m_IsLoaded = false;
    };
}  // namespace Ethereal
