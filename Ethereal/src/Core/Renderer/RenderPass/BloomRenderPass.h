#pragma once
#include "Core/Renderer/RenderPass.h"

namespace Ethereal {
class BloomRenderPass : public RenderPass {
   public:
    struct ConfigUib {
        float Threshold;
        float Knee;
        float Intensity;
        bool DownSample;
        uint32_t MipLevel;
    };
    virtual void Init(uint32_t width, uint32_t height) override;
    virtual void Draw() override;
    virtual void OnResize(uint32_t width, uint32_t height) override;

    void SetMainImage(const TextureHandle& texture) { m_MainImage = texture; }
    TextureHandle GetBloomImage() { return m_BloomImage; }

   private:
    TextureHandle m_MainImage;
    TextureHandle m_BloomImage;
    TextureHandle m_BrightAreaImage;

    TextureHandle m_UpSampledImage;
    TextureHandle m_DownSampledImage;

    RenderTargetHandle mRenderTarget;
    RenderPassParams mParams;
    PipelineState mBrightPipeline;
    PipelineState mBlurPipeline;
    PipelineState mMergePipeline;

    TypedUniform<ConfigUib> mUniformInterfaceBuffer;
    BufferObjectHandle mUniformBuffer;

    SamplerGroupHandle mSamplerGroup;
    SamplerGroupDescriptor mSamplerGroupDesc;

    Ref<StaticMesh> m_Quad;
    unsigned int m_MipLevels;

    uint32_t m_Height, m_Width;

   private:
    void Invalidate(uint32_t width, uint32_t height);
};

}  // namespace Ethereal
