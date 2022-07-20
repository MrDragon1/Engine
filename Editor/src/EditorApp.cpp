#include <Ethereal.hpp>
#include <Ethereal/Core/EntryPoint.hpp>

#include "Platform/Linux/OpenGL/OpenGLShader.hpp"

#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EditorLayer.hpp"

namespace Ethereal
{
    class Editor : public Application {
      public:
        Editor() : Application("Engine Editor") {
            PushLayer(new EditorLayer());
        }
        
        ~Editor() {
        }
    };

    Application* CreateApplication() {
        return new Editor();
    };
}