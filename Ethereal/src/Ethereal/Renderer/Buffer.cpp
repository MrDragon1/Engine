#include "pch.hpp"
#include "Buffer.hpp"

#include "RendererAPI.hpp"

#include "Platform/Linux/OpenGL/OpenGLBuffer.hpp"

namespace Ethereal
{

    Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:    ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return Ref<OpenGLVertexBuffer>::Create(size);
        }
        ET_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::None:    ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL:  return Ref<OpenGLVertexBuffer>::Create(vertices, size);
        }
        ET_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::None:    ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL:  return Ref<OpenGLIndexBuffer>::Create(indices, size);
        }
        ET_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
} 