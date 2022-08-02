#include <Ethereal.h>
#include "Core/EntryPoint.h"

#include "Platform/Windows/OpenGL/OpenGLShader.h"

#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EditorLayer.h"

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