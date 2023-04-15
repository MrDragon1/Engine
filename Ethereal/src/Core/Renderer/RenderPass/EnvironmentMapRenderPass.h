#pragma once
#include "Core/Renderer/RenderPass.h"

namespace Ethereal {
class EnvironmentMapRenderPass : public RenderPass {
   public:
    struct ConfigUib {
        Matrix4 ProjectionMatrix;
        Matrix4 ViewMatrix;
        float Roughness;
    };
    EnvironmentMapRenderPass() = default;
    ~EnvironmentMapRenderPass() = default;
    void Init(uint32_t width, uint32_t height) override;
    void Draw() override;
    void OnResize(uint32_t width, uint32_t height) override;

   private:
    Ref<StaticMesh> mCube;

    TextureHandle mEnvironmentCubeMap;
    TextureHandle mReflectionCubeMap;

    TextureHandle mTextureCube;
    TextureHandle mInputTexture;  // not assign value now

    RenderPassParams mParams = {};
    PipelineState mEquirectangularToCubeMapPipeline;
    PipelineState mPrefilterPipeline;
    PipelineState mConvolutionPipeline;
    RenderTargetHandle mRenderTarget;

    TypedUniform<ConfigUib> mUniformInterfaceBuffer;
    BufferObjectHandle mUniformBuffer;

    SamplerGroupHandle mSamplerGroup;
    SamplerGroupDescriptor mSamplerGroupDesc;
    friend class RenderSystem;
};
}  // namespace Ethereal