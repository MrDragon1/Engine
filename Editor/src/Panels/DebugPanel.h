#pragma once
#include "Core/Editor/EditorPanel.h"

namespace Ethereal {
class DebugPanel : public EditorPanel {
   public:
    DebugPanel();

    void OnImGuiRender(bool& isOpen) override;
    void OnEvent(Event& event) override;

   private:
};
}  // namespace Ethereal