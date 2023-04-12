#include <Ethereal.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Base/EntryPoint.h"
#include "EditorLayer.h"
#include "imgui.h"

namespace Ethereal {
class Editor : public Application {
   public:
    Editor() : Application("Engine Editor") { PushLayer(new EditorLayer()); }

    ~Editor() {}
};

Application* CreateApplication() { return new Editor(); };
}  // namespace Ethereal