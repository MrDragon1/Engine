#pragma once
#include "Core/Editor/EditorPanel.h"
#include "Core/Material/MaterialGraph.h"
namespace ed = ax::NodeEditor;

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

   private:
    // Resource for blueprint
    ed::EditorContext* mEditor = nullptr;

    struct NodeIdLess {
        bool operator()(const NodeID& lhs, const NodeID& rhs) const { return lhs < rhs; }
    };

    const int mPinIconSize = 24;
    const float mTouchTime = 1.0f;
    std::map<NodeID, float, NodeIdLess> mNodeTouchTime;

   private:
    void TouchNode(NodeID id);
    float GetTouchProgress(NodeID id);
    void UpdateTouch();
    bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2,
                  float splitter_long_axis_size = -1.0f);
    void DrawPinIcon(const MaterialPin& pin, bool connected, int alpha);
    ImColor GetIconColor(MaterialPinType type);
    void ShowLeftPane(float paneWidth);
};

}  // namespace Ethereal