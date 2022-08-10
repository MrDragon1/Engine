#pragma once

#include "Renderer/RendererAPI.h"

namespace Ethereal
{
    class OpenGLRendererAPI : public RendererAPI {
      public:
        virtual void Init() override;
        virtual void SetClearColor(const glm::vec4& color) override;
        virtual void Clear() override;
        virtual ETHEREAL_DRAW_MODE GetDrawMode() override;
        virtual void SetCullFace(ETHEREAL_CULLFACE_TYPE type) override;
        virtual void SetDrawMode(ETHEREAL_DRAW_MODE mode) override;
        virtual void SetDepthFunc(ETHEREAL_DEPTH_FUNC func) override;
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0, uint32_t start = 0, uint32_t offset = 0) override;
        virtual void Draw(const Ref<VertexArray>& vertexArray, uint32_t count = 0) override;

      private:
        ETHEREAL_DRAW_MODE m_DrawMode = ETHEREAL_DRAW_MODE::ETHEREAL_DRAW_MODE_FILLED;
        GLenum m_DrawModeGL = GL_TRIANGLES;
    };
}  // namespace Ethereal