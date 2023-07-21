#pragma once
#include "Core/Renderer/RenderPass.h"

namespace Ethereal {
class TestRenderPass : public RenderPass {
   public:
    TestRenderPass() = default;
    ~TestRenderPass() = default;
    void Init(uint32_t width, uint32_t height) override;
    void Draw() override;
    void OnResize(uint32_t width, uint32_t height) override;

   private:
    friend class RenderSystem;

    RenderPassParams mParams = {};
    PipelineState mStaticMeshPipeline;
    PipelineState mMeshPipeline;
    PipelineState mSkyboxPipeline;
    RenderTargetHandle mRenderTarget;
};
}  // namespace Ethereal
