#pragma once

#include "Core/Renderer/RendererAPI.h"

namespace Ethereal
{
    class RenderCommand {
      public:
        inline static void Init() { s_RendererAPI->Init(); }

        inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { s_RendererAPI->SetViewport(x, y, width, height); }

        inline static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }

        inline static void Clear() { s_RendererAPI->Clear(); }

        inline static void SetDrawMode(ETHEREAL_DRAW_MODE mode) { s_RendererAPI->SetDrawMode(mode); }

        inline static ETHEREAL_DRAW_MODE GetDrawMode() { return s_RendererAPI->GetDrawMode(); }

        inline static void SetDepthFunc(ETHEREAL_DEPTH_FUNC func) { s_RendererAPI->SetDepthFunc(func); }

        inline static void SetCullFace(ETHEREAL_CULLFACE_TYPE type) { s_RendererAPI->SetCullFace(type); }

        inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0, void* indices = nullptr, uint32_t offset = 0) {
            s_RendererAPI->DrawIndexed(vertexArray, count, indices, offset);
        }

        inline static void Draw(const Ref<VertexArray>& vertexArray, uint32_t count = 0) { s_RendererAPI->Draw(vertexArray, count); }

      private:
        static RendererAPI* s_RendererAPI;
    };
}  // namespace Ethereal