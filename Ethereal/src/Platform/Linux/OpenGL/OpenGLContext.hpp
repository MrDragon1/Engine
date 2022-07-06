#pragma once

#include "Ethereal/Renderer/GraphicsContext.hpp"

struct GLFWwindow;

namespace Ethereal {
class OpenGLContext : public GraphicsContext {
  public:
    OpenGLContext(GLFWwindow *windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;

  private:
    GLFWwindow *m_WindowHandle;
};
}  // namespace Ethereal