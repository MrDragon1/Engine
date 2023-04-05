#include "Buffer.h"

#include "Platform/Windows/OpenGL/OpenGLBuffer.h"
#include "RendererAPI.h"
#include "pch.h"

namespace Ethereal {

Ref<VertexBuffer> VertexBuffer::Create(uint32_t size) {
    switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return Ref<OpenGLVertexBuffer>::Create(size);
    }
    ET_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

Ref<VertexBuffer> VertexBuffer::Create(void* vertices, uint32_t size) {
    switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return Ref<OpenGLVertexBuffer>::Create(vertices, size);
    }
    ET_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

Ref<IndexBuffer> IndexBuffer::Create(void* indices, uint32_t size) {
    switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return Ref<OpenGLIndexBuffer>::Create(indices, size);
    }
    ET_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

Ref<UniformBuffer_> UniformBuffer_::Create(uint32_t size, uint32_t binding) {
    switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return Ref<OpenGLUniformBuffer>::Create(size, binding);
    }
    ET_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

Ref<UniformBufferSet> UniformBufferSet::Create(uint32_t frames) {
    switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return Ref<OpenGLUniformBufferSet>::Create(frames);
    }
    ET_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}
}  // namespace Ethereal