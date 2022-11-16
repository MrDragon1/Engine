#pragma once
#include "Core/Renderer/RenderPass.h"

namespace Ethereal
{
    class SkyboxRenderPass : public RenderPass {
      public:
        SkyboxRenderPass() = default;
        ~SkyboxRenderPass() = default;
        void Init(uint32_t width, uint32_t height) override;
        void Draw() override;
        void OnResize(uint32_t width, uint32_t height) override;

        void SetSkyboxProjection(const Matrix4& matrix) { m_SkyboxProjection = matrix; };
        void SetSkyboxView(const Matrix4& matrix) { m_SkyboxView = matrix; };

      private:
        Ref<Shader> m_SkyboxShader;
        std::string m_SkyboxShaderPath = "assets/shaders/Skybox.glsl";
        Matrix4 m_SkyboxProjection;
        Matrix4 m_SkyboxView;
        Ref<StaticMesh> m_Cube;

        friend class RenderSystem;
    };
}  // namespace Ethereal