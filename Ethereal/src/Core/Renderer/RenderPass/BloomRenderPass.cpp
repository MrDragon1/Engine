#include "BloomRenderPass.h"

#include <Ethereal/src/Base/GlobalContext.h>

#include "Core/Renderer/RenderResource.h"
namespace Ethereal {
void BloomRenderPass::Init(uint32_t width, uint32_t height) {
    mQuad = RenderResource::Quad;

    auto api = GlobalContext::GetDriverApi();
    auto usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLEABLE;
    auto hdrTex = api->CreateTexture(1, width, height, 1, TextureFormat::R32G32B32A32_HDR, usage,
                                     TextureType::TEXTURE_2D);
    mRenderTarget =
        api->CreateRenderTarget(TargetBufferFlags::COLOR0, width, height, {hdrTex}, {}, {});

    ShaderSource source;
    source[ShaderType::VERTEX] = BRIGHT_VERT;
    source[ShaderType::FRAGMENT] = BRIGHT_FRAG;
    mBrightPipeline.program = api->CreateProgram("BRIGHT", source);

    source[ShaderType::VERTEX] = BLUR_VERT;
    source[ShaderType::FRAGMENT] = BLUR_FRAG;
    mBlurPipeline.program = api->CreateProgram("BLUR", source);

    source[ShaderType::VERTEX] = MERGE_VERT;
    source[ShaderType::FRAGMENT] = MERGE_FRAG;
    mMergePipeline.program = api->CreateProgram("MERGE", source);

    mUniformBuffer = api->CreateBufferObject(mUniformInterfaceBuffer.GetSize(),
                                             BufferObjectBinding::UNIFORM, BufferUsage::STATIC);
    api->UpdateBufferObject(mUniformBuffer, mUniformInterfaceBuffer.ToBufferDescriptor(), 0);

    mSamplerGroup = api->CreateSamplerGroup(3);
    mSamplerGroupDesc = SamplerGroupDescriptor(3);

    Invalidate(width, height);
}

void BloomRenderPass::Draw() {
    if (mMainImage == nullptr) {
        ET_CORE_WARN("BloomRenderPass::Draw() - MainImage is nullptr");
        return;
    }
    auto api = GlobalContext::GetDriverApi();

    mParams.clearColor = Vector4{0.0, 0.0, 0.0, 0.0};
    auto& s = mUniformInterfaceBuffer.Edit();
    s.Threshold = Project::GetConfigManager().sBloomConfig.Threshold;
    s.Intensity = Project::GetConfigManager().sBloomConfig.Intensity;
    s.Knee = Project::GetConfigManager().sBloomConfig.Knee;

    api->BeginRenderPass(mRenderTarget, mParams);

    // Draw BrightArea Image
    {
        s.Threshold = 0.0;
        /*
         * Notice that we are not initialize .level and .layer,
         * otherwise, the backend will take this texture as image2D, not sampler2D in shader (like
         * we do in blur image stage below)
         */
        mSamplerGroupDesc[0] = SamplerDescriptor{
            .texture = mMainImage,
            .params = SamplerParams::Default(),
            .binding = 0,
        };
        api->SetRenderTargetAttachment(mRenderTarget,
                                       {.handle = mBrightAreaImage, .level = 0, .layer = 0},
                                       TargetBufferFlags::COLOR0);

        api->UpdateBufferObject(mUniformBuffer, mUniformInterfaceBuffer.ToBufferDescriptor(), 0);
        api->BindUniformBuffer(0, mUniformBuffer);

        api->UpdateSamplerGroup(mSamplerGroup, mSamplerGroupDesc);
        api->BindSamplerGroup(0, mSamplerGroup);

        api->Draw(mQuad->GetMeshSource()->GetRenderPrimitive(), mBrightPipeline);
    }

    // Blur Image
    {
        // Down Sampler
        s.Threshold = Project::GetConfigManager().sBloomConfig.Threshold;
        s.Knee = Project::GetConfigManager().sBloomConfig.Knee;
        s.DownSample = true;

        // For 0 mip level
        s.MipLevel = 0;
        mSamplerGroupDesc[0] = SamplerDescriptor{
            .texture = mDownSampledImage,
            .params = SamplerParams::Default(),
            .binding = 0,
            .level = 0,
            .layer = 0,
        };
        mSamplerGroupDesc[1] = SamplerDescriptor{
            .texture = mBrightAreaImage,
            .params = SamplerParams::Default(),
            .binding = 1,
            .level = 0,
            .layer = 0,
        };
        api->UpdateBufferObject(mUniformBuffer, mUniformInterfaceBuffer.ToBufferDescriptor(), 0);
        api->BindUniformBuffer(0, mUniformBuffer);

        api->UpdateSamplerGroup(mSamplerGroup, mSamplerGroupDesc);
        api->BindSamplerGroup(0, mSamplerGroup);

        api->Draw(mQuad->GetMeshSource()->GetRenderPrimitive(), mBlurPipeline);

        // For other mip level
        for (int i = 1; i < mMipLevels; i++) {
            auto mipWidth = static_cast<unsigned int>(mWidth * std::pow(0.5, i));
            auto mipHeight = static_cast<unsigned int>(mHeight * std::pow(0.5, i));

            s.MipLevel = i;
            mSamplerGroupDesc[0] = SamplerDescriptor{
                .texture = mDownSampledImage,
                .params = SamplerParams::Default(),
                .binding = 0,
                .level = static_cast<uint8_t>(i),
                .layer = 0,
            };
            mSamplerGroupDesc[1] = SamplerDescriptor{
                .texture = mDownSampledImage,
                .params = SamplerParams::Default(),
                .binding = 1,
                .level = static_cast<uint8_t>(i - 1),
                .layer = 0,
            };

            api->UpdateBufferObject(mUniformBuffer, mUniformInterfaceBuffer.ToBufferDescriptor(),
                                    0);
            api->BindUniformBuffer(0, mUniformBuffer);

            api->UpdateSamplerGroup(mSamplerGroup, mSamplerGroupDesc);
            api->BindSamplerGroup(0, mSamplerGroup);

            api->Draw(mQuad->GetMeshSource()->GetRenderPrimitive(), mBlurPipeline);
        }

        // Up Sampler
        auto mipWidth = static_cast<unsigned int>(mWidth * std::pow(0.5, mMipLevels - 2));
        auto mipHeight = static_cast<unsigned int>(mHeight * std::pow(0.5, mMipLevels - 2));

        s.DownSample = false;
        s.MipLevel = mMipLevels - 2;

        mSamplerGroupDesc[0] = SamplerDescriptor{
            .texture = mUpSampledImage,
            .params = SamplerParams::Default(),
            .binding = 0,
            .level = static_cast<uint8_t>(mMipLevels - 2),
            .layer = 0,
        };
        mSamplerGroupDesc[1] = SamplerDescriptor{
            .texture = mDownSampledImage,
            .params = SamplerParams::Default(),
            .binding = 1,
            .level = static_cast<uint8_t>(mMipLevels - 1),
            .layer = 0,
        };
        mSamplerGroupDesc[2] = SamplerDescriptor{
            .texture = mDownSampledImage,
            .params = SamplerParams::Default(),
            .binding = 2,
            .level = static_cast<uint8_t>(mMipLevels - 2),
            .layer = 0,
        };

        api->UpdateBufferObject(mUniformBuffer, mUniformInterfaceBuffer.ToBufferDescriptor(), 0);
        api->BindUniformBuffer(0, mUniformBuffer);

        api->UpdateSamplerGroup(mSamplerGroup, mSamplerGroupDesc);
        api->BindSamplerGroup(0, mSamplerGroup);

        api->Draw(mQuad->GetMeshSource()->GetRenderPrimitive(), mBlurPipeline);
        for (int i = mMipLevels - 3; i >= 0; --i) {
            mipWidth = static_cast<unsigned int>(mWidth * std::pow(0.5, i));
            mipHeight = static_cast<unsigned int>(mHeight * std::pow(0.5, i));
            s.MipLevel = i;

            mSamplerGroupDesc[0] = SamplerDescriptor{
                .texture = mUpSampledImage,
                .params = SamplerParams::Default(),
                .binding = 0,
                .level = static_cast<uint8_t>(i),
                .layer = 0,
            };
            mSamplerGroupDesc[1] = SamplerDescriptor{
                .texture = mUpSampledImage,
                .params = SamplerParams::Default(),
                .binding = 1,
                .level = static_cast<uint8_t>(i + 1),
                .layer = 0,
            };
            mSamplerGroupDesc[2] = SamplerDescriptor{
                .texture = mDownSampledImage,
                .params = SamplerParams::Default(),
                .binding = 2,
                .level = static_cast<uint8_t>(i),
                .layer = 0,
            };

            api->UpdateBufferObject(mUniformBuffer, mUniformInterfaceBuffer.ToBufferDescriptor(),
                                    0);
            api->BindUniformBuffer(0, mUniformBuffer);

            api->UpdateSamplerGroup(mSamplerGroup, mSamplerGroupDesc);
            api->BindSamplerGroup(0, mSamplerGroup);

            api->Draw(mQuad->GetMeshSource()->GetRenderPrimitive(), mBlurPipeline);
        }
    }

    // Merge Image
    {
        s.Intensity = Project::GetConfigManager().sBloomConfig.Intensity;
        mSamplerGroupDesc[0] = SamplerDescriptor{
            .texture = mMainImage,
            .params = SamplerParams::Default(),
            .binding = 0,
        };

        mSamplerGroupDesc[1] = SamplerDescriptor{
            .texture = mUpSampledImage,
            .params = SamplerParams::Default(),
            .binding = 1,
        };

        api->SetRenderTargetAttachment(mRenderTarget,
                                       {.handle = mBloomImage, .level = 0, .layer = 0},
                                       TargetBufferFlags::COLOR0);
        api->UpdateBufferObject(mUniformBuffer, mUniformInterfaceBuffer.ToBufferDescriptor(), 0);
        api->BindUniformBuffer(0, mUniformBuffer);

        api->UpdateSamplerGroup(mSamplerGroup, mSamplerGroupDesc);
        api->BindSamplerGroup(0, mSamplerGroup);

        api->Draw(mQuad->GetMeshSource()->GetRenderPrimitive(), mMergePipeline);
    }
    api->EndRenderPass();
}

void BloomRenderPass::OnResize(uint32_t width, uint32_t height) { Invalidate(width, height); }

void BloomRenderPass::Invalidate(uint32_t width, uint32_t height) {
    mHeight = height;
    mWidth = width;
    mMipLevels = log(std::min(height, width)) / log(2);

    auto api = GlobalContext::GetDriverApi();
    auto usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLEABLE;
    auto hdrTex = api->CreateTexture(1, width, height, 1, TextureFormat::R32G32B32A32_HDR, usage,
                                     TextureType::TEXTURE_2D);

    mBrightAreaImage = api->CreateTexture(1, width, height, 1, TextureFormat::R32G32B32A32_HDR,
                                          usage, TextureType::TEXTURE_2D);
    mBloomImage = api->CreateTexture(1, width, height, 1, TextureFormat::R32G32B32A32_HDR, usage,
                                     TextureType::TEXTURE_2D);

    mUpSampledImage =
        api->CreateTexture(mMipLevels, width, height, 1, TextureFormat::R32G32B32A32_HDR, usage,
                           TextureType::TEXTURE_2D);
    api->GenerateMipmaps(mUpSampledImage);

    mDownSampledImage =
        api->CreateTexture(mMipLevels, width, height, 1, TextureFormat::R32G32B32A32_HDR, usage,
                           TextureType::TEXTURE_2D);
    api->GenerateMipmaps(mDownSampledImage);

    // This will resize the render target
    api->SetRenderTargetAttachment(mRenderTarget, {.handle = mBloomImage, .level = 0, .layer = 0},
                                   TargetBufferFlags::COLOR0);
}
}  // namespace Ethereal