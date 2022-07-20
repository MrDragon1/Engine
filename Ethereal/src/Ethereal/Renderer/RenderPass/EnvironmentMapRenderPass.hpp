#pragma once
#include "Ethereal/Renderer/RenderPass.hpp"

namespace Ethereal
{
    class EnvironmentMapRenderPass : public RenderPass {
      public:
        EnvironmentMapRenderPass() = default;
        ~EnvironmentMapRenderPass() = default;
        void Init(uint32_t width, uint32_t height) override;
        void Draw() override;
        void OnResize(uint32_t width, uint32_t height) override;

      private:
        bool m_IsFirstCall = true;
        Ref<Framebuffer> m_Framebuffer;

        Ref<Shader> m_EquirectangularToCubeMapShader;
        std::string m_EquirectangularToCubeMapShaderPath = "assets/shaders/EquirectangularToCubeMap.glsl";
        Ref<Shader> m_IrradianceConvolutionShader;
        std::string m_IrradianceConvolutionShaderPath = "assets/shaders/IrradianceConvolution.glsl";

        Ref<TextureCube> m_EnvCubeMap;
        Ref<TextureCube> m_IrradianceCubeMap;
        Ref<Texture> m_HDRTexture;

        std::string m_HDRTexturePath = "assets/textures/hdr/newport_loft.hdr";

        GLMesh m_Cube;

        friend class RenderSystem;
    };
}  // namespace Ethereal