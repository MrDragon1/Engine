#pragma once
#include "Common.hpp"
#include "Ethereal/Renderer/Shader.hpp"

namespace Ethereal
{
    struct VisiableNodes {
        std::vector<RenderNode>* p_directional_light_visible_mesh_nodes{nullptr};
        std::vector<RenderNode>* p_point_lights_visible_mesh_nodes{nullptr};
        std::vector<RenderNode>* p_main_camera_visible_mesh_nodes{nullptr};
    };
    class RenderPass {
      public:
        RenderPass() = default;
        virtual ~RenderPass() = default;
        virtual void Init() = 0;
        virtual void Draw() = 0;

        static VisiableNodes m_VisiableNodes;
    };

    class MainCameraRenderPass : public RenderPass {
      public:
        MainCameraRenderPass() = default;
        ~MainCameraRenderPass() = default;
        void Init() override;
        void Draw() override;

        void SetViewProjectionMatrix(const glm::mat4& matrix) { m_ViewProjectionMatrix = matrix; }

      private:
        glm::mat4 m_ViewProjectionMatrix;
        Ref<Shader> m_Shader;
        std::string m_ShaderPath = "assets/shaders/Test.glsl";
    };
}  // namespace Ethereal