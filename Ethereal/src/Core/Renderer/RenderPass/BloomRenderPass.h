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

    void SetMainImage(const TextureHandle& texture) { mMainImage = texture; }
    TextureHandle GetBloomImage() { return mBloomImage; }

   private:
    TextureHandle mMainImage;
    TextureHandle mBloomImage;
    TextureHandle mBrightAreaImage;

    TextureHandle mUpSampledImage;
    TextureHandle mDownSampledImage;

    RenderTargetHandle mRenderTarget;
    RenderPassParams mParams;
    PipelineState mBrightPipeline;
    PipelineState mBlurPipeline;
    PipelineState mMergePipeline;

    TypedUniform<ConfigUib> mUniformInterfaceBuffer;
    BufferObjectHandle mUniformBuffer;

    SamplerGroupHandle mSamplerGroup;
    SamplerGroupDescriptor mSamplerGroupDesc;

    Ref<StaticMesh> mQuad;
    unsigned int mMipLevels;

    uint32_t mHeight, mWidth;

   private:
    void Invalidate(uint32_t width, uint32_t height);
};

}  // namespace Ethereal
