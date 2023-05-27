#pragma once
#include "Core/Editor/EditorPanel.h"
#include "Core/Material/MaterialGraph.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
#include <stack>
namespace ed = ax::NodeEditor;

namespace Ethereal {
class MaterialGraphPanel : public EditorPanel {
   public:
    MaterialGraphPanel();
    ~MaterialGraphPanel();

    void OnImGuiRender(bool& isOpen) override;
    void OnEvent(Event& event) override;
    void SetGraph(MaterialGraphPtr graph);
    void PopGraph();

   private:
    DocumentPtr mDocument;
    MaterialGraphPtr mCurrentGraph = nullptr;
    std::stack<MaterialGraphPtr> mGraphStack;
    std::unordered_map<std::string, std::vector<NodeDefinePtr>> mBuildinNodeDef;
    bool mAutoLayout = false;
    /// UI stuff
    ed::EditorContext* mEditor = nullptr;

    struct NodeIdLess {
        bool operator()(const NodeID& lhs, const NodeID& rhs) const { return lhs < rhs; }
    };

    const int mPinIconSize = 24;

   private:
    ///
    void ShowLeftPanel(float paneWidth);
    void ShowUINodeInspectorPanel(float panelWidth, float panelHeight);
    void ShowPreviewPanel(float panelWidth, float panelHeight);
    MaterialNodePtr CreateNodeMenu();
    void PrepareNodeMenuInfo();
    void DrawPinController(MaterialPinPtr pin);
    void Compile();
    void ResetLayout();
    /// Utils
    bool Splitter(bool split_vertically, float thickness, float* size1, float* size2,
                  float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);
    void DrawPinIcon(MaterialPinPtr pin, bool connected, int alpha);
    ImColor GetIconColor(MaterialPinType type);
    const string PinTypeToString(MaterialPinType type);
    MaterialPinType GetPinType(const string& type);
};

}  // namespace Ethereal