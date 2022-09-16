#pragma once
#include "Renderer/RenderPass.h"
#include "CSMRenderPass.h"

namespace Ethereal
{
    class MainCameraRenderPass : public RenderPass {
      public:
        MainCameraRenderPass() = default;
        ~MainCameraRenderPass() = default;
        void Init(uint32_t width, uint32_t height) override;
        void Draw() override;
        void OnResize(uint32_t width, uint32_t height) override;

        void SetViewProjectionMatrix(const glm::mat4& matrix) { m_ViewProjectionMatrix = matrix; }
        int GetMousePicking(int x, int y);

        void SetCameraPosition(const glm::vec3& position) { m_CameraPosition = position; };

        void SetCSMData(CSMRenderPassData data) { m_CSMData = data; }

      private:
        glm::mat4 m_ViewProjectionMatrix;

        Ref<Shader> m_Shader;
        std::string m_ShaderPath = "assets/shaders/PBR.glsl";
        Ref<Framebuffer> m_Framebuffer;

        // temporary
        glm::vec3 m_CameraPosition;

        CSMRenderPassData m_CSMData;

        friend class RenderSystem;
    };
}  // namespace Ethereal