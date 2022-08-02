#include "pch.h"
#include "VertexArray.h"

#include "RendererAPI.h"
#include "Platform/Windows/OpenGL/OpenGLVertexArray.h"

namespace Ethereal
{
    Ref<VertexArray> VertexArray::Create() {
            switch (RendererAPI::GetAPI()) {
                case RendererAPI::API::None:
                    ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
                    return nullptr;
                case RendererAPI::API::OpenGL:
                    return Ref<OpenGLVertexArray>::Create();
            }
        ET_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}  // namespace Ethereal