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

        void SetViewProjectionMatrix(const Matrix4x4& matrix) { m_ViewProjectionMatrix = matrix; }
        int GetMousePicking(int x, int y);

        void SetCameraPosition(const Vector3& position) { m_CameraPosition = position; };

        void SetCSMData(CSMRenderPassData data) { m_CSMData = data; }

      private:
        Matrix4x4 m_ViewProjectionMatrix;

        Ref<Shader> m_StaticMeshShader;
        std::string m_StaticMeshShaderPath = "assets/shaders/PBR.glsl";

        Ref<Shader> m_MeshShader;
        std::string m_MeshShaderPath = "assets/shaders/PBR_Anim.glsl";

        Ref<Framebuffer> m_Framebuffer;

        // temporary
        Vector3 m_CameraPosition;

        CSMRenderPassData m_CSMData;

        friend class RenderSystem;
    };
}  // namespace Ethereal