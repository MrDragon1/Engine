#pragma once
#include "Base/Event/Event.h"
#include "Core/Scene/Scene.h"
#include "Core/Project/Project.h"

namespace Ethereal {

class EditorPanel : public RefCounted {
   public:
    virtual ~EditorPanel() = default;

    virtual void OnImGuiRender(bool& isOpen) = 0;
    virtual void OnEvent(Event& e) {}
    virtual void OnUpdate(TimeStamp ts){};
    virtual void OnProjectChanged(const Ref<Project>& project) {}
    virtual void SetSceneContext(const Ref<Scene>& context) {}
};

}  // namespace Ethereal
