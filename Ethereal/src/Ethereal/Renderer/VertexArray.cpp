#include "pch.hpp"
#include "VertexArray.hpp"

#include "Renderer.hpp"
#include "Platform/Linux/OpenGL/OpenGLVertexArray.hpp"

namespace Ethereal
{
    Ref<VertexArray> VertexArray::Create() {
            switch (Renderer::GetAPI()) {
                case RendererAPI::API::None:
                    ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
                    return nullptr;
                case RendererAPI::API::OpenGL:
                    return std::make_shared<OpenGLVertexArray>();
            }
        ET_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}  // namespace Ethereal