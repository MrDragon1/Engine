#include "MaterialGraphPanel.h"
#include "imgui.h"
#include <imgui_node_editor/imgui_node_editor.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace ed = ax::NodeEditor;
static ed::EditorContext* g_Context = nullptr;  // Editor context, required to trace a editor state.
static bool g_FirstFrame = true;                // Flag set for first frame only, some action need to be executed once.

namespace Ethereal {
void ImGuiEx_BeginColumn() { ImGui::BeginGroup(); }

void ImGuiEx_NextColumn() {
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
}

void ImGuiEx_EndColumn() { ImGui::EndGroup(); }

MaterialGraphPanel::MaterialGraphPanel() {
    g_Context = ed::CreateEditor();

    mGraph = Ref<MaterialGraph>::Create();
    mGraph->SetName("Test Material");

    mGraph->AddNode(TextureNode(GenerateNodeID(), "Texture 1"));

    mGraph->AddNode(TextureNode(GenerateNodeID(), "Texture 2"));

    mGraph->AddLink(GenerateLinkID(), mGraph->GetNode("Texture 1").Outputs[0].ID, mGraph->GetNode("Texture 2").Inputs[0].ID);
}

MaterialGraphPanel::~MaterialGraphPanel() { ed::DestroyEditor(g_Context); }

void MaterialGraphPanel::OnImGuiRender(bool& isOpen) {
    ImGui::SetNextWindowSize(ImVec2(200.0f, 300.0f), ImGuiCond_Appearing);
    ImGui::Begin("Material Graph Editor", nullptr);

    auto& io = ImGui::GetIO();

    ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

    ImGui::Separator();

    ed::SetCurrentEditor(g_Context);

    // Start interaction with editor.
    ed::Begin(mGraph->GetName().c_str(), ImVec2(0.0, 0.0f));

    int uniqueId = 1;

    //
    // 1) Commit known data to editor
    //
    for (auto& [ID, node] : mGraph->GetNodes()) {
        if (g_FirstFrame) ed::SetNodePosition(ID, ImVec2(10 * ID, 10 * ID));
        ed::BeginNode(ID);
        ImGui::Text(node.Name.c_str());
        auto pos = ImGui::GetCursorPos();
        for (auto& pin : node.Inputs) {
            ed::BeginPin(pin.ID, ed::PinKind::Input);
            ImGui::Text(pin.Name.c_str());
            ed::EndPin();
        }
        ImGui::SetCursorPos(pos);
        ImGui::SameLine();
        for (auto& pin : node.Outputs) {
            ed::BeginPin(pin.ID, ed::PinKind::Output);
            ImGui::Text(pin.Name.c_str());
            ed::EndPin();
        }
        ed::EndNode();
    }

    // Submit Links
    for (auto& [ID, link] : mGraph->GetLinks()) {
        ed::Link(link.ID, link.InputID, link.OutputID);
    }

    //
    // 2) Handle interactions
    //

    // Handle creation action, returns true if editor want to create new object (node or link)
    if (ed::BeginCreate()) {
        ed::PinId inputPinId, outputPinId;
        if (ed::QueryNewLink(&inputPinId, &outputPinId)) {
            // QueryNewLink returns true if editor want to create new link between pins.
            //
            // Link can be created only for two valid pins, it is up to you to
            // validate if connection make sense. Editor is happy to make any.
            //
            // Link always goes from input to output. User may choose to drag
            // link from output pin or input pin. This determine which pin ids
            // are valid and which are not:
            //   * input valid, output invalid - user started to drag new ling from input pin
            //   * input invalid, output valid - user started to drag new ling from output pin
            //   * input valid, output valid   - user dragged link over other pin, can be validated

            if (inputPinId && outputPinId)  // both are valid, let's accept link
            {
                // ed::AcceptNewItem() return true when user release mouse button.
                if (ed::AcceptNewItem()) {
                    // Since we accepted new link, lets add one to our list of links.
                    auto linkID = GenerateLinkID();
                    mGraph->AddLink(linkID, (PinID)inputPinId, (PinID)outputPinId);
                    // Draw new link.
                    ed::Link(linkID, inputPinId, outputPinId);
                }

                // You may choose to reject connection between these nodes
                // by calling ed::RejectNewItem(). This will allow editor to give
                // visual feedback by changing link thickness and color.
            }
        }
    }
    ed::EndCreate();  // Wraps up object creation action handling.

    // Handle deletion action
    if (ed::BeginDelete()) {
        // There may be many links marked for deletion, let's loop over them.
        ed::LinkId deletedLinkId;
        while (ed::QueryDeletedLink(&deletedLinkId)) {
            // If you agree that link can be deleted, accept deletion.
            if (ed::AcceptDeletedItem()) {
                // Then remove link from your data.
                mGraph->RemoveLink((LinkID)deletedLinkId);
            }

            // You may reject link deletion by calling:
            // ed::RejectDeletedItem();
        }
    }
    ed::EndDelete();  // Wrap up deletion action

    // End of interaction with editor.
    ed::End();

    if (g_FirstFrame) ed::NavigateToContent(0.0f);

    ed::SetCurrentEditor(nullptr);

    g_FirstFrame = false;

    ImGui::End();
}

void MaterialGraphPanel::OnEvent(Event& event) {}

}  // namespace Ethereal
