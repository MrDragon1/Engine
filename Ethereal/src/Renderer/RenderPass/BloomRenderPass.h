#pragma once
#include "Renderer/RenderPass.h"

namespace Ethereal
{
    class BloomRenderPass : public RenderPass {
      public:
        virtual void Init(uint32_t width, uint32_t height) override;
        virtual void Draw() override;
        virtual void OnResize(uint32_t width, uint32_t height) override;

        void SetMainImage(const Ref<Texture2D>& texture) { m_MainImage = texture; }
        Ref<Texture2D> GetBloomImage() { return m_BloomImage; }

      private:
        Ref<Shader> m_Shader_Bright;
        Ref<Shader> m_Shader_Blur;
        Ref<Shader> m_Shader_Merge;

        Ref<Framebuffer> m_Framebuffer;

        Ref<Texture2D> m_MainImage;
        Ref<Texture2D> m_BloomImage;
        Ref<Texture2D> m_BrightAreaImage;
        Ref<Texture2D> m_BlurImage[2];

        Ref<Texture2D> m_UpSampledImage;
        Ref<Texture2D> m_DownSampledImage;

        Ref<StaticMesh> m_Quad;
        unsigned int m_MipLevels;

        uint32_t m_Height, m_Width;

      private:
        void Invalidate(uint32_t width, uint32_t height);
    };

}  // namespace Ethereal
