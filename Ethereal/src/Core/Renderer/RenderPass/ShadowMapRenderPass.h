#pragma once
#include "Core/Renderer/RenderPass.h"

namespace Ethereal {
class ShadowMapRenderPass : public RenderPass {
   public:
    ShadowMapRenderPass() = default;
    ~ShadowMapRenderPass() = default;
    void Init(uint32_t width, uint32_t height) override;
    void Draw() override;

    void OnResize(uint32_t width, uint32_t height) override;
    void SetLightPosition(const Vector3& pos) {
        mLightPos = pos;
        CalculateViewProjectionMatrix();
    }

   private:
    Matrix4 mViewProjectionMatrix;

    Vector3 mLightPos;
    const int mShadowMapSize = 1024;

    RenderPassParams mParams = {};
    PipelineState mPipelineState;
    RenderTargetHandle mRenderTarget;

    friend class RenderSystem;

   private:
    void CalculateViewProjectionMatrix();
};
}  // namespace Ethereal