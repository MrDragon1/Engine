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
        m_LightPos = pos;
        CalculateViewProjectionMatrix();
    }

   private:
    Matrix4 m_ViewProjectionMatrix;

    Vector3 m_LightPos;
    const int m_ShadowMapSize = 1024;

    RenderPassParams mParams = {};
    PipelineState mPipelineState;
    RenderTargetHandle mRenderTarget;

    friend class RenderSystem;

   private:
    void CalculateViewProjectionMatrix();
};
}  // namespace Ethereal