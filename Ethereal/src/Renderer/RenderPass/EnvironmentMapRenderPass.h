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

        Ref<TextureCube> m_BackgroundCubeMap;
        Ref<TextureCube> m_EnvironmentCubeMap;
        Ref<TextureCube> m_ReflectionCubeMap;
        Ref<Texture> m_BRDFLUTTexture;

        Ref<Texture> m_BackgroundTexture;
        Ref<Texture> m_EnvironmentTexture;
        Ref<Texture> m_ReflectionTexture;
        Ref<TextureCube> m_ReflectionCubeMapNoMipmaps;

        std::string m_BackgroundTexturePath;
        std::string m_EnvironmentTexturePath;
        std::string m_ReflectionTexturePath;

        std::string m_BRDFLUTTexturePath = "assets/textures/global/brdf_schilk.hdr";

        Ref<StaticMesh> m_Cube;

        friend class RenderSystem;
    };
}  // namespace Ethereal