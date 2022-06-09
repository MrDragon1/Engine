#include "OpenGLContext.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Engine/Core/Utils.hpp"
#include "pch.hpp"

namespace Engine {
OpenGLContext::OpenGLContext(GLFWwindow *windowHandle) : m_WindowHandle(windowHandle) { ENGINE_CORE_ASSERT(windowHandle, "Window handle is null!"); }

void OpenGLContext::Init() {
    glfwMakeContextCurrent(m_WindowHandle);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    ENGINE_CORE_ASSERT(status, "Failed to initialize Glad!");
}

void OpenGLContext::SwapBuffers() { glfwSwapBuffers(m_WindowHandle); }
}  // namespace Engine