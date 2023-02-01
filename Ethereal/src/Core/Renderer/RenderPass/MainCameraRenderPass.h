#pragma once
#include "Core/Renderer/RenderPass.h"
#include "Core/Renderer/RenderPass/CSMRenderPass.h"

namespace Ethereal
{
    class MainCameraRenderPass : public RenderPass {
      public:
        MainCameraRenderPass() = default;
        ~MainCameraRenderPass() = default;
        void Init(uint32_t width, uint32_t height) override;
        void Draw() override;
        void OnResize(uint32_t width, uint32_t height) override;

        int GetMousePicking(int x, int y);

        void SetCSMData(CSMRenderPassData data) { m_CSMData = data; }

      private:
        Ref<Shader> m_StaticMeshShader;
        std::string m_StaticMeshShaderPath = "assets/shaders/PBR.glsl";

        Ref<Shader> m_MeshShader;
        std::string m_MeshShaderPath = "assets/shaders/PBR_Anim.glsl";

        Ref<Framebuffer> m_Framebuffer;

        CSMRenderPassData m_CSMData;

        friend class RenderSystem;
    };
}  // namespace Ethereal