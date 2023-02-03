#pragma once
#include "Core/Renderer/RenderPass.h"

namespace Ethereal
{
    struct CSMRenderPassData {
        int ShadowMapSize = 4096;

        float FOV;
        float AspectRatio;

        Vector3 LightDir;

        Ref<Texture> ShadowMap;
    };

    class CSMRenderPass : public RenderPass {
      public:
        CSMRenderPass() = default;
        ~CSMRenderPass() = default;

        void Init(uint32_t width, uint32_t height) override;
        void Draw() override;

        void OnResize(uint32_t width, uint32_t height) override;
        void SetNearFarPlane(float nearPlane, float farPlane);
        void SetFOV(const float fov) { m_Data.FOV = fov; }
        void SetAspectRatio(const float ratio) { m_Data.AspectRatio = ratio; }

        void SetLightDir(const Vector3& dir) { m_Data.LightDir = Math::Normalize(dir); }

        CSMRenderPassData GetData() { return m_Data; }
        void BindFramebuffer() { m_Framebuffer->Bind(); }
      private:
        std::vector<Vector4> GetFrustumCornersWorldSpace(const Matrix4& projview);
        std::vector<Vector4> GetFrustumCornersWorldSpace(const Matrix4& proj, const Matrix4& view);
        Matrix4 GetLightSpaceMatrix(float nearPlane, float farPlane);
        void CalculateLightSpaceMatrices();

      private:
        Ref<Shader> m_Shader;

        Ref<Framebuffer> m_Framebuffer;

        std::vector<float> m_Distance;

        CSMRenderPassData m_Data;
    };

}  // namespace Ethereal
