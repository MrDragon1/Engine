#include "pch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Ethereal
{
    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
        switch (type) {
            case Ethereal::ShaderDataType::Float:
                return GL_FLOAT;
            case Ethereal::ShaderDataType::Float2:
                return GL_FLOAT;
            case Ethereal::ShaderDataType::Float3:
                return GL_FLOAT;
            case Ethereal::ShaderDataType::Float4:
                return GL_FLOAT;
            case Ethereal::ShaderDataType::Mat3:
                return GL_FLOAT;
            case Ethereal::ShaderDataType::Mat4:
                return GL_FLOAT;
            case Ethereal::ShaderDataType::Int:
                return GL_INT;
            case Ethereal::ShaderDataType::Int2:
                return GL_INT;
            case Ethereal::ShaderDataType::Int3:
                return GL_INT;
            case Ethereal::ShaderDataType::Int4:
                return GL_INT;
            case Ethereal::ShaderDataType::Bool:
                return GL_BOOL;
        }

        ET_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    OpenGLVertexArray::OpenGLVertexArray() { glCreateVertexArrays(1, &m_RendererID); }

    OpenGLVertexArray::~OpenGLVertexArray() { glDeleteVertexArrays(1, &m_RendererID); }

    void OpenGLVertexArray::Bind() const {
        glBindVertexArray(m_RendererID);
        m_IndexBuffer->Bind();
    }

    void OpenGLVertexArray::Unbind() const { glBindVertexArray(0); }

    void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
        ET_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout!");

        glBindVertexArray(m_RendererID);
        vertexBuffer->Bind();

        uint32_t index = 0;
        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout) {
            switch (element.Type) {
                case ShaderDataType::Float:
                case ShaderDataType::Float2:
                case ShaderDataType::Float3:
                case ShaderDataType::Float4: {
                    glEnableVertexAttribArray(index);
                    glVertexAttribPointer(index, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type),
                                          element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)element.Offset);
                    index++;
                    break;
                }
                case ShaderDataType::Int:
                case ShaderDataType::Int2:
                case ShaderDataType::Int3:
                case ShaderDataType::Int4:
                case ShaderDataType::Bool: {
                    glEnableVertexAttribArray(index);
                    glVertexAttribIPointer(index, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), layout.GetStride(),
                                           (const void*)element.Offset);
                    index++;
                    break;
                }
                case ShaderDataType::Mat3:
                case ShaderDataType::Mat4: {
                    uint8_t count = element.GetComponentCount();
                    for (uint8_t i = 0; i < count; i++) {
                        glEnableVertexAttribArray(index);
                        glVertexAttribPointer(index, count, ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized ? GL_TRUE : GL_FALSE,
                                              layout.GetStride(), (const void*)(element.Offset + sizeof(float) * count * i));
                        glVertexAttribDivisor(index, 1);
                        index++;
                    }
                    break;
                }
                default:
                    ET_CORE_ASSERT(false, "Unknown ShaderDataType!");
            }
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
        glBindVertexArray(m_RendererID);
        indexBuffer->Bind();

        m_IndexBuffer = indexBuffer;
    }
}  // namespace Ethereal