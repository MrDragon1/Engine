#include "pch.hpp"
#include "RenderCommand.hpp"

#include "Platform/Linux/OpenGL/OpenGLRendererAPI.hpp"

namespace Ethereal
{
    RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}