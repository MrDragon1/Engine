#pragma once
#include "Core/Renderer/RenderPass.h"
#include "Core/Material/MaterialCore.h"
namespace Ethereal {
class MaterialPreviewRenderPass : public RenderPass {
   public:
    MaterialPreviewRenderPass() = default;
    ~MaterialPreviewRenderPass() = default;
    void Init(uint32_t width, uint32_t height) override;
    void Draw() override;
    void OnResize(uint32_t width, uint32_t height) override;

    void SetMaterial(MaterialCorePtr mat);
    TextureHandle GetMainImage() { return mRenderTarget->color[0]; }

   private:
    friend class RenderSystem;
    Ref<StaticMesh> mObject;

    RenderPassParams mParams = {};
    PipelineState mPipeline;
    PipelineState mSkyboxPipeline;
    RenderTargetHandle mRenderTarget;
};
}  // namespace Ethereal
