#pragma once
#include "Core/Renderer/RenderPass.h"
#include "Core/Renderer/RenderPass/CSMRenderPass.h"

namespace Ethereal {
class MainCameraRenderPass : public RenderPass {
   public:
    MainCameraRenderPass() = default;
    ~MainCameraRenderPass() = default;
    void Init(uint32_t width, uint32_t height) override;
    void Draw() override;
    void OnResize(uint32_t width, uint32_t height) override;

    int GetMousePicking(int x, int y);

   private:
    friend class RenderSystem;

    RenderPassParams mParams = {};
    PipelineState mStaticMeshPipeline;
    PipelineState mMeshPipeline;
    PipelineState mSkyboxPipeline;
    RenderTargetHandle mRenderTarget;
};
}  // namespace Ethereal