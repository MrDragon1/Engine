#include "pch.hpp"
#include "VertexArray.hpp"

#include "Renderer.hpp"
#include "Platform/Linux/OpenGL/OpenGLVertexArray.hpp"

namespace Engine
{
    VertexArray* VertexArray::Create()
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::None:    ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::OpenGL:  return new OpenGLVertexArray();
        }
        ENGINE_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}