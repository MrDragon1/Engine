#pragma once
#include "Core/Editor/EditorPanel.h"

namespace Ethereal {
class MaterialGraphPanel : public EditorPanel {
   public:
    MaterialGraphPanel();
    ~MaterialGraphPanel();

    void OnImGuiRender(bool& isOpen) override;
    void OnEvent(Event& event) override;

   private:
   private:
};

}  // namespace Ethereal