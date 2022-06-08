#include "pch.hpp"
#include "RenderCommand.hpp"

#include "Platform/Linux/OpenGL/OpenGLRendererAPI.hpp"

namespace Engine
{
    RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}