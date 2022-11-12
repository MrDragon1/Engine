#pragma once

#include <glm/glm.hpp>
#include "Utils/Macro.h"
#include "Core/Renderer/VertexArray.h"

namespace Ethereal
{
    class RendererAPI {
      public:
        enum class API
        {
            None = 0,
            OpenGL = 1,
        };

      public:
        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void Clear() = 0;
        virtual void Init() = 0;
        virtual ETHEREAL_DRAW_MODE GetDrawMode() = 0;
        virtual void SetCullFace(ETHEREAL_CULLFACE_TYPE type) = 0;
        virtual void SetDrawMode(ETHEREAL_DRAW_MODE mode) = 0;
        virtual void SetDepthFunc(ETHEREAL_DEPTH_FUNC func) = 0;
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0, void* indices = nullptr, uint32_t offset = 0) = 0;
        virtual void Draw(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

        inline static API GetAPI() { return s_API; }

      private:
        static API s_API;
    };
}  // namespace Ethereal