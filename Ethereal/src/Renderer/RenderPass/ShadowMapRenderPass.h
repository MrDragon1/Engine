#pragma once
#include "Renderer/RenderPass.h"

namespace Ethereal
{
    class ShadowMapRenderPass : public RenderPass {
      public:
        ShadowMapRenderPass() = default;
        ~ShadowMapRenderPass() = default;
        void Init(uint32_t width, uint32_t height) override;
        void Draw() override;

        void OnResize(uint32_t width, uint32_t height) override;
        void SetLightPosition(const glm::vec3& pos) {
            m_LightPos = pos;
            CalculateViewProjectionMatrix();
        }

      private:
        glm::mat4 m_ViewProjectionMatrix;

        glm::vec3 m_LightPos;
        Ref<Shader> m_Shader;
        std::string m_ShaderPath = "assets/shaders/ShadowMap.glsl";
        Ref<Framebuffer> m_Framebuffer;
        const int m_ShadowMapSize = 1024;

        friend class RenderSystem;

      private:
        void CalculateViewProjectionMatrix();
    };
}  // namespace Ethereal