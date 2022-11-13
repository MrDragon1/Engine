#pragma once
#include "Core/Renderer/RenderPass.h"

namespace Ethereal
{
    struct CSMRenderPassData {
        int Cascaded = 4;
        int ShadowMapSize = 4096;

        Matrix4x4 View;
        Matrix4x4 Proj;
        float FOV;
        float AspectRatio;
        float NearPlane;
        float FarPlane;

        Vector3 LightDir;

        Ref<Texture> ShadowMap;
        std::vector<float> Distance;
        std::vector<Matrix4x4> LightMatrices;
    };

    class CSMRenderPass : public RenderPass {
      public:
        CSMRenderPass() = default;
        ~CSMRenderPass() = default;

        void Init(uint32_t width, uint32_t height) override;
        void Draw() override;

        void OnResize(uint32_t width, uint32_t height) override;
        void SetNearFarPlane(float nearPlane, float farPlane) {
            m_Data.NearPlane = nearPlane;
            m_Data.FarPlane = farPlane;
            // TODO: automatically generate distance according to m_Cascaded
            m_Distance.clear();
            m_Distance.push_back(m_Data.FarPlane / 50.0f);
            m_Distance.push_back(m_Data.FarPlane / 25.0f);
            m_Distance.push_back(m_Data.FarPlane / 10.0f);
            m_Distance.push_back(m_Data.FarPlane / 2.0f);
        }
        void SetViewMatrix(const Matrix4x4& view) { m_Data.View = view; }
        void SetProjMatrix(const Matrix4x4& proj) { m_Data.Proj = proj; }
        void SetFOV(const float fov) { m_Data.FOV = fov; }
        void SetAspectRatio(const float ratio) { m_Data.AspectRatio = ratio; }

        void SetLightDir(const Vector3& dir) { m_Data.LightDir = dir.normalisedCopy(); }

        CSMRenderPassData GetData() { return m_Data; }

      private:
        std::vector<Vector4> GetFrustumCornersWorldSpace(const Matrix4x4& projview);
        std::vector<Vector4> GetFrustumCornersWorldSpace(const Matrix4x4& proj, const Matrix4x4& view);
        Matrix4x4 GetLightSpaceMatrix(float nearPlane, float farPlane);
        std::vector<Matrix4x4> GetLightSpaceMatrices();

      private:
        Ref<Shader> m_Shader;
        const std::string m_ShaderPath = "assets/shaders/CSM/CSM.glsl";
        Ref<Framebuffer> m_Framebuffer;

        std::vector<float> m_Distance;

        CSMRenderPassData m_Data;
    };

}  // namespace Ethereal
