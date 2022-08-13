#pragma once
#include "Renderer/RenderPass.h"

namespace Ethereal
{
    class BloomRenderPass : public RenderPass {
      public:
        virtual void Init(uint32_t width, uint32_t height) override;
        virtual void Draw() override;
        virtual void OnResize(uint32_t width, uint32_t height) override;

        void SetMainImage(const Ref<Texture>& texture) { m_MainImage = texture; }
        Ref<Texture> GetBloomImage() { return m_BloomImage; }

      private:
        Ref<Shader> m_Shader_Bright;
        Ref<Shader> m_Shader_Blur;
        Ref<Shader> m_Shader_Merge;

        Ref<Framebuffer> m_Framebuffer;

        Ref<Texture> m_MainImage;
        Ref<Texture> m_BloomImage;
        Ref<Texture> m_BrightAreaImage;
        Ref<Texture> m_BlurImage[2];

        Ref<StaticMesh> m_Quad;

      private:
        void Invalidate(uint32_t width, uint32_t height);
    };

}  // namespace Ethereal
