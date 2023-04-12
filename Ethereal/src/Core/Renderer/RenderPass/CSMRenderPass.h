#pragma once
#include "Core/Renderer/RenderPass.h"

namespace Ethereal {

class CSMRenderPass : public RenderPass {
   public:
    CSMRenderPass() = default;
    ~CSMRenderPass() = default;

    void Init(uint32_t width, uint32_t height) override;
    void Draw() override;

    void OnResize(uint32_t width, uint32_t height) override;
    void UpdateDistance();

   private:
    std::vector<Vector4> GetFrustumCornersWorldSpace(const Matrix4& projview);
    std::vector<Vector4> GetFrustumCornersWorldSpace(const Matrix4& proj, const Matrix4& view);
    Matrix4 GetLightSpaceMatrix(float nearPlane, float farPlane);
    void CalculateLightSpaceMatrices();

   private:
    RenderPassParams mParams = {};
    PipelineState mPipelineState;
    RenderTargetHandle mRenderTarget;

    std::vector<float> m_Distance;
};

}  // namespace Ethereal
