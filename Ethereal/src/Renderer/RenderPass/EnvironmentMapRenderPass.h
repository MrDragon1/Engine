#pragma once
#include "Renderer/RenderPass.h"

namespace Ethereal
{
    class EnvironmentMapRenderPass : public RenderPass {
      public:
        EnvironmentMapRenderPass() = default;
        ~EnvironmentMapRenderPass() = default;
        void Init(uint32_t width, uint32_t height) override;
        void Draw() override;
        void OnResize(uint32_t width, uint32_t height) override;

        void Reset() { m_IsFirstCall = true; };

      private:
        bool m_IsFirstCall = true;
        Ref<Framebuffer> m_Framebuffer;

        Ref<Shader> m_EquirectangularToCubeMapShader;
        std::string m_EquirectangularToCubeMapShaderPath = "assets/shaders/EquirectangularToCubeMap.glsl";
        Ref<Shader> m_PrefilterShader;
        std::string m_PrefilterShaderPath = "assets/shaders/Prefilter.glsl";
        Ref<Shader> m_ConvolutionShader;
        std::string m_ConvolutionShaderPath = "assets/shaders/IrradianceConvolution.glsl";

        Ref<TextureCube> m_EnvironmentCubeMap;
        Ref<TextureCube> m_ReflectionCubeMap;

        Ref<Texture> m_Texture;
        Ref<TextureCube> m_TextureCube;
        std::string m_Path;

        Ref<StaticMesh> m_Cube;

        friend class RenderSystem;
    };
}  // namespace Ethereal