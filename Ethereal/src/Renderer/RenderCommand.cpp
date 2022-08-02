#include "pch.h"
#include "RenderCommand.h"

#include "Platform/Windows/OpenGL/OpenGLRendererAPI.h"

namespace Ethereal
{
    RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}