#pragma once
#include "Core/Editor/EditorPanel.h"
#include "Core/Material/MaterialGraph.h"
namespace Ethereal {
class MaterialGraphPanel : public EditorPanel {
   public:
    MaterialGraphPanel();
    ~MaterialGraphPanel();

    void OnImGuiRender(bool& isOpen) override;
    void OnEvent(Event& event) override;
    void SetGraph(Ref<MaterialGraph> graph) { mGraph = graph; }

   private:
    Ref<MaterialGraph> mGraph;
};

}  // namespace Ethereal