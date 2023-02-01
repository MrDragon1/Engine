#include "pch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Ethereal
{
    void OpenGLRendererAPI::Init() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
    }

    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { glViewport(x, y, width, height); }

    void OpenGLRendererAPI::SetClearColor(const Vector4& color) { glClearColor(color.x, color.y, color.z, color.w); }

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

    void OpenGLRendererAPI::EnableCullFace(bool enable)
    {
        if (enable) glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);
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
    
    void OpenGLRendererAPI::SetDepthFunc(ETHEREAL_DEPTH_FUNC func)
    {
        switch (func) {
            case ETHEREAL_DEPTH_FUNC::LESS:
                glDepthFunc(GL_LESS);
                break;
            case ETHEREAL_DEPTH_FUNC::LEQUAL:
                glDepthFunc(GL_LEQUAL);
                break;
            case ETHEREAL_DEPTH_FUNC::EQUAL:
                glDepthFunc(GL_EQUAL);
                break;
            case ETHEREAL_DEPTH_FUNC::GEQUAL: 
                glDepthFunc(GL_GEQUAL);
                break;  
            case ETHEREAL_DEPTH_FUNC::GREATER:
                glDepthFunc(GL_GREATER);
                break;
            case ETHEREAL_DEPTH_FUNC::NOTEQUAL:
                glDepthFunc(GL_NOTEQUAL);
                break;
            case ETHEREAL_DEPTH_FUNC::ALWAYS:
                glDepthFunc(GL_ALWAYS);
                break;
            case ETHEREAL_DEPTH_FUNC::NEVER:
                glDepthFunc(GL_NEVER);
                break;
            default:
                ET_CORE_ASSERT("Unknown depth func");
                break;
        }
    }

    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount, void* indices, uint32_t offset) {
        uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
        vertexArray->Bind();
        // @pram: indices a pointer to the offset value in the indexbuffer
        // e.g:  offset * sizeof(uint32_t)
        glDrawElementsBaseVertex(m_DrawModeGL, count, GL_UNSIGNED_INT, indices, offset);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenGLRendererAPI::Draw(const Ref<VertexArray>& vertexArray, uint32_t count) {
        ET_CORE_ASSERT(count, "glDrawArrays Count is 0");
        vertexArray->Bind();
        glDrawArrays(m_DrawModeGL, 0, count);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
}  // namespace Ethereal