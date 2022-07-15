#pragma once
#include "Common.hpp"
#include "Ethereal/Renderer/Shader.hpp"
#include "Ethereal/Renderer/FrameBuffer.hpp"

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
        virtual void Init(uint32_t width, uint32_t height) = 0;
        virtual void Draw() = 0;
        virtual void OnResize(uint32_t width, uint32_t height) = 0;

        static VisiableNodes m_VisiableNodes;

        friend class RenderSystem;
    };

    class MainCameraRenderPass : public RenderPass {
      public:
        MainCameraRenderPass() = default;
        ~MainCameraRenderPass() = default;
        void Init(uint32_t width, uint32_t height) override;
        void Draw() override;
        void OnResize(uint32_t width, uint32_t height) override;

        void SetViewProjectionMatrix(const glm::mat4& matrix) { m_ViewProjectionMatrix = matrix; }
        int GetMousePicking(int x, int y);

        void SetLightSpaceMatrix(const glm::mat4& matrix) { m_LightSpaceMatrix = matrix; };

      private:
        glm::mat4 m_ViewProjectionMatrix;
        glm::mat4 m_LightSpaceMatrix;

        Ref<Shader> m_Shader;
        std::string m_ShaderPath = "assets/shaders/Test.glsl";
        Ref<Framebuffer> m_Framebuffer;

        friend class RenderSystem;
    };

    class ShadowMapRenderPass : public RenderPass {
      public:
        ShadowMapRenderPass() = default;
        ~ShadowMapRenderPass() = default;
        void Init(uint32_t width, uint32_t height) override;
        void Draw() override;

        void OnResize(uint32_t width, uint32_t height) override;
        void SetLightPosition(const glm::vec3& pos) { m_LightPos = pos; CalculateViewProjectionMatrix();}

      private:
        glm::mat4 m_ViewProjectionMatrix;

        glm::vec3 m_LightPos;
        Ref<Shader> m_Shader;
        std::string m_ShaderPath = "assets/shaders/ShadowMap.glsl";
        Ref<Framebuffer> m_Framebuffer;
        const int m_ShadowMapSize = 1024;

        friend class RenderSystem;
        
        private: 
        void CalculateViewProjectionMatrix();
    };

}  // namespace Ethereal