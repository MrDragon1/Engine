#pragma once
#include "Renderer/RenderPass.h"

namespace Ethereal
{
    class CSMRenderPass : public RenderPass {
      public:
        CSMRenderPass() = default;
        ~CSMRenderPass() = default;

        void Init(uint32_t width, uint32_t height) override;
        void Draw() override;

        void OnResize(uint32_t width, uint32_t height) override;
        void SetNearFarPlane(float nearPlane, float farPlane) {
            m_NearPlane = nearPlane;
            m_FarPlane = farPlane;
        }
        void SetViewMatrix(const glm::mat4& view) { m_View = view; }
        void SetProjMatrix(const glm::mat4& proj) { m_Proj = proj; }
        void SetFOV(const float fov) { m_FOV = fov; }
        void SetAspectRatio(const float ratio) { m_AspectRatio = ratio; }

      private:
        std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& projview);
        std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
        glm::mat4 GetLightSpaceMatrix(const float nearPlane, const float farPlane);
        std::vector<glm::mat4> GetLightSpaceMatrices();

      private:
        const int m_Cascaded = 4;
        const int m_ShadowMapSize = 1024;

        Ref<Shader> m_Shader;
        const std::string m_ShaderPath = "assets/shaders/CSM/CSM.glsl";
        Ref<Framebuffer> m_Framebuffer;
        Ref<Texture3D> m_ShadowMaps;

        std::vector<int> m_Distance;

        glm::mat4 m_View;
        glm::mat4 m_Proj;
        float m_FOV;
        float m_AspectRatio;
        float m_NearPlane = 0.1f;
        float m_FarPlane = 500.0f;

        glm::vec3 m_LightDir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
    };

}  // namespace Ethereal
