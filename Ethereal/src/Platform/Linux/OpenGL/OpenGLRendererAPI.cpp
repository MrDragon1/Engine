#include "pch.hpp"
#include "OpenGLRendererAPI.hpp"

#include <glad/glad.h>

namespace Ethereal
{
    void OpenGLRendererAPI::Init() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
    }

    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { glViewport(x, y, width, height); }

    void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) { glClearColor(color.r, color.g, color.b, color.a); }

    void OpenGLRendererAPI::Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
    
    ETHEREAL_DRAW_MODE OpenGLRendererAPI::GetDrawMode()
    {
        return m_DrawMode;
    }
    
    void OpenGLRendererAPI::SetCullFace(ETHEREAL_CULLFACE_TYPE type)
    {
        GLenum cullface;
        switch (type) {
            case ETHEREAL_CULLFACE_TYPE::BACK: {
                cullface = GL_BACK;
                break;
            }
            case ETHEREAL_CULLFACE_TYPE::FRONT:{
                cullface = GL_FRONT;
                break;
            }

            default:
                ET_CORE_ASSERT("Unknown cullface type");

        }
        glCullFace(cullface);
    }

    void OpenGLRendererAPI::SetDrawMode(ETHEREAL_DRAW_MODE mode) {
        m_DrawMode = mode;
        switch (mode) {
            case ETHEREAL_DRAW_MODE::ETHEREAL_DRAW_MODE_FILLED:
                m_DrawModeGL = GL_TRIANGLES;
                break;
            case ETHEREAL_DRAW_MODE::ETHEREAL_DRAW_MODE_LINE:
                m_DrawModeGL = GL_LINES;
                break;
            case ETHEREAL_DRAW_MODE::ETHEREAL_DRAW_MODE_POINT:
                m_DrawModeGL = GL_POINTS;
                break;
        };
    }

    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) {
        uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
        vertexArray->Bind();
        glDrawElements(m_DrawModeGL, count, GL_UNSIGNED_INT, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenGLRendererAPI::Draw(const Ref<VertexArray>& vertexArray, uint32_t count) {
        ET_CORE_ASSERT(count, "glDrawArrays Count is 0");
        vertexArray->Bind();
        glDrawArrays(m_DrawModeGL, 0, count);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
}  // namespace Ethereal