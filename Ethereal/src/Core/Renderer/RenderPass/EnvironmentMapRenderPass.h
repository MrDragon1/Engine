#pragma once
#include "Core/Renderer/RenderPass.h"

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
        Ref<Framebuffer> m_Framebuffer;

        Ref<Shader> m_EquirectangularToCubeMapShader;
        Ref<Shader> m_PrefilterShader;
        Ref<Shader> m_ConvolutionShader;

        Ref<TextureCube> m_EnvironmentCubeMap;
        Ref<TextureCube> m_ReflectionCubeMap;

        Ref<Texture> m_Texture;
        Ref<TextureCube> m_TextureCube;
        std::string m_Path;

        Ref<StaticMesh> m_Cube;

        friend class RenderSystem;
    };
}  // namespace Ethereal