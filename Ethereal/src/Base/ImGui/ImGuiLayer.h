#pragma once

#include "Base/Layer/Layer.h"

#include "Base/Event/ApplicationEvent.h"
#include "Base/Event/KeyEvent.h"
#include "Base/Event/MouseEvent.h"

#include "Backend/DriverEnum.h"
namespace Ethereal {
class ImGuiLayer : public Layer {
   public:
    ImGuiLayer();
    static Ref<ImGuiLayer> Create(BackendType backend);

    virtual void BlockEvents(bool block) { mBlockEvents = block; }
    virtual void Begin() = 0;
    virtual void End() = 0;

   protected:
    virtual void SetDarkThemeColors();

   private:
    float mTime = 0.0f;
    bool mBlockEvents = true;
};
}  // namespace Ethereal