#pragma once
#include "Common.hpp"
#include "Ethereal/Renderer/Shader.hpp"
#include "Ethereal/Renderer/FrameBuffer.hpp"
#include "RenderCommand.hpp"

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

}  // namespace Ethereal