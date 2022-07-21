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

        void Reset() { m_IsFirstCall = true; };

      private:
        bool m_IsFirstCall = true;
        Ref<Framebuffer> m_Framebuffer;

        Ref<Shader> m_EquirectangularToCubeMapShader;
        std::string m_EquirectangularToCubeMapShaderPath = "assets/shaders/EquirectangularToCubeMap.glsl";
        Ref<Shader> m_PrefilterShader;
        std::string m_PrefilterShaderPath = "assets/shaders/Prefilter.glsl";

        Ref<TextureCube> m_EnvCubeMap;
        Ref<TextureCube> m_PrefilterCubeMap;
        Ref<TextureCube> m_IrradianceCubeMap;
        Ref<Texture> m_BRDFLUTTexture;

        Ref<Texture> m_EnvTexture;
        Ref<Texture> m_IrradianceTexture;

        std::string m_EnvTexturePath = "assets/textures/skybox/Newport_Loft/Newport_Loft_Ref.hdr";
        std::string m_IrradianceTexturePath = "assets/textures/skybox/Newport_Loft/Newport_Loft_Env.hdr";
        std::string m_BRDFLUTTexturePath = "assets/textures/global/brdf_schilk.hdr";

        GLMesh m_Cube;

        friend class RenderSystem;
    };
}  // namespace Ethereal