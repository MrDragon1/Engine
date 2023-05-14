#include "MaterialGraphPanel.h"
#include "imgui.h"
#include <imgui_node_editor/imgui_node_editor.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "Base/GlobalContext.h"

#include "Utils/XmlParser.h"
#include "Core/Material/MaterialBase/Document.h"
namespace Ethereal {

MaterialGraphPanel::MaterialGraphPanel() {
    mEditor = ed::CreateEditor();
    ed::SetCurrentEditor(mEditor);

    ed::NavigateToContent();

    mGraph = Ref<MaterialGraph>::Create();
    mGraph->SetName("Test Material");

    mGraph->AddNode(TextureNode(GenerateNodeID(), "Texture 1"));

    mGraph->AddNode(TextureNode(GenerateNodeID(), "Texture 2"));

    // mGraph->AddLink(GenerateLinkID(), mGraph->GetNode("Texture 1").Outputs[0].ID,
    // mGraph->GetNode("Texture 2").Inputs[0].ID);

    DocumentPtr doc = DocumentPtr::Create(nullptr, "doc1");
    Xml::LoadDocument(doc, "assets/materials/material.mtlx");
    Xml::LoadLibraries(doc);
    doc->Validate();
    auto ni = doc->GetChild("SR_marble1").As<NodeInstance>();
    auto ng = doc->GetChildren(MaterialElementType::NODEGRAPH);
}

MaterialGraphPanel::~MaterialGraphPanel() {
    if (mEditor) {
        ed::DestroyEditor(mEditor);
        mEditor = nullptr;
    }
}

void MaterialGraphPanel::OnImGuiRender(bool& isOpen) {
    ImGui::SetNextWindowSize(ImVec2(200.0f, 300.0f), ImGuiCond_Appearing);
    ImGui::Begin("Material Graph Editor", nullptr);

    UpdateTouch();

    auto& io = ImGui::GetIO();

    auto pos = ImGui::GetCursorScreenPos();
    ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

    ed::SetCurrentEditor(mEditor);

    static ed::NodeId contextNodeId = 0;
    static ed::LinkId contextLinkId = 0;
    static ed::PinId contextPinId = 0;
    static bool createNewNode = false;
    static MaterialPin* newNodeLinkPin = nullptr;
    static MaterialPin* newLinkPin = nullptr;

    static float leftPaneWidth = 400.0f;
    static float rightPaneWidth = 800.0f;
    Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);

    ShowLeftPane(leftPaneWidth - 4.0f);

    ImGui::SameLine(0.0f, 12.0f);
    ed::Begin("Node editor");
    {
        auto cursorTopLeft = ImGui::GetCursorScreenPos();
        auto api = GlobalContext::GetDriverApi();
        auto bg = EditorResource::MaterialGraphHeaderBG;
        BlueprintNodeBuilder builder((ImTextureID)api->GetTextueID(bg), bg->width, bg->height);

        for (auto& [id, node] : mGraph->GetNodes()) {
            if (node.Type != NodeType::Blueprint && node.Type != NodeType::Simple) continue;

            const auto isSimple = node.Type == NodeType::Simple;

            builder.Begin(node.ID);
            if (!isSimple) {
                builder.Header(ImVec4(node.Color.x, node.Color.y, node.Color.z, 1.0));
                ImGui::Spring(0);
                ImGui::TextUnformatted(node.Name.c_str());
                ImGui::Spring(1);
                ImGui::Dummy(ImVec2(0, 28));
                ImGui::Spring(0);
                builder.EndHeader();
            }

            for (auto& input : node.Inputs) {
                auto alpha = ImGui::GetStyle().Alpha;
                if (newLinkPin && false && &input != newLinkPin) alpha = alpha * (48.0f / 255.0f);

                builder.Input(input.ID);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                DrawPinIcon(input, mGraph->IsPinLinked(input.ID), (int)(alpha * 255));
                ImGui::Spring(0);
                if (!input.Name.empty()) {
                    ImGui::TextUnformatted(input.Name.c_str());
                    ImGui::Spring(0);
                }
                if (input.Type == MaterialPinType::Bool) {
                    ImGui::Button("Hello");
                    ImGui::Spring(0);
                }
                ImGui::PopStyleVar();
                builder.EndInput();
            }

            if (isSimple) {
                builder.Middle();

                ImGui::Spring(1, 0);
                ImGui::TextUnformatted(node.Name.c_str());
                ImGui::Spring(1, 0);
            }

            for (auto& output : node.Outputs) {
                if (!isSimple && false) continue;

                auto alpha = ImGui::GetStyle().Alpha;
                if (newLinkPin && !true && &output != newLinkPin) alpha = alpha * (48.0f / 255.0f);

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                builder.Output(output.ID);

                if (output.Type == MaterialPinType::String) {
                    static char buffer[128] = "Edit Me\nMultiline!";
                    static bool wasActive = false;

                    ImGui::PushItemWidth(100.0f);
                    ImGui::InputText("##edit", buffer, 127);
                    ImGui::PopItemWidth();
                    if (ImGui::IsItemActive() && !wasActive) {
                        ed::EnableShortcuts(false);
                        wasActive = true;
                    } else if (!ImGui::IsItemActive() && wasActive) {
                        ed::EnableShortcuts(true);
                        wasActive = false;
                    }
                    ImGui::Spring(0);
                }

                if (!output.Name.empty()) {
                    ImGui::Spring(0);
                    ImGui::TextUnformatted(output.Name.c_str());
                }
                ImGui::Spring(0);
                DrawPinIcon(output, mGraph->IsPinLinked(output.ID), (int)(alpha * 255));

                ImGui::PopStyleVar();
                builder.EndOutput();
            }

            builder.End();
        }

        for (auto& [id, link] : mGraph->GetLinks()) ed::Link(link.ID, link.InputID, link.OutputID);

        if (!createNewNode) {
            if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f)) {
                auto showLabel = [](const char* label, ImColor color) {
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                    auto size = ImGui::CalcTextSize(label);

                    auto padding = ImGui::GetStyle().FramePadding;
                    auto spacing = ImGui::GetStyle().ItemSpacing;

                    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                    auto rectMin = ImGui::GetCursorScreenPos() - padding;
                    auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                    auto drawList = ImGui::GetWindowDrawList();
                    drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                    ImGui::TextUnformatted(label);
                };

                ed::PinId startPinId = 0, endPinId = 0;
                if (ed::QueryNewLink(&startPinId, &endPinId)) {
                    auto startPin = mGraph->GetPin((PinID)startPinId);
                    auto endPin = mGraph->GetPin((PinID)endPinId);

                    newLinkPin = startPin ? startPin : endPin;

                    if (startPin->Kind == PinKind::Input) {
                        std::swap(startPin, endPin);
                        std::swap(startPinId, endPinId);
                    }

                    if (startPin && endPin) {
                        if (endPin == startPin) {
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        } else if (endPin->Kind == startPin->Kind) {
                            showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        // else if (endPin->Node == startPin->Node)
                        //{
                        //     showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                        //     ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                        // }
                        else if (endPin->Type != startPin->Type) {
                            showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                        } else {
                            showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                                mGraph->AddLink(GenerateLinkID(), (PinID)startPinId,
                                                (PinID)endPinId);
                                // s_Links.back().Color = GetIconColor(startPin->Type);
                            }
                        }
                    }
                }

                ed::PinId pinId = 0;
                if (ed::QueryNewNode(&pinId)) {
                    newLinkPin = mGraph->GetPin((PinID)pinId);
                    if (newLinkPin) showLabel("+ Create Node", ImColor(32, 45, 32, 180));

                    if (ed::AcceptNewItem()) {
                        createNewNode = true;
                        newNodeLinkPin = mGraph->GetPin((PinID)pinId);
                        newLinkPin = nullptr;
                        ed::Suspend();
                        ImGui::OpenPopup("Create New Node");
                        ed::Resume();
                    }
                }
            } else
                newLinkPin = nullptr;

            ed::EndCreate();

            if (ed::BeginDelete()) {
                ed::LinkId linkId = 0;
                while (ed::QueryDeletedLink(&linkId)) {
                    if (ed::AcceptDeletedItem()) {
                        mGraph->RemoveLink((LinkID)linkId);
                    }
                }

                ed::NodeId nodeId = 0;
                while (ed::QueryDeletedNode(&nodeId)) {
                    if (ed::AcceptDeletedItem()) {
                        mGraph->RemoveNode((NodeID)nodeId);
                    }
                }
            }
            ed::EndDelete();
        }

        ImGui::SetCursorScreenPos(cursorTopLeft);
    }

    // deal with popup
    auto openPopupPosition = ImGui::GetMousePos();

    ed::Suspend();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("Create New Node")) {
        auto newNodePostion = openPopupPosition;
        // ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

        // auto drawList = ImGui::GetWindowDrawList();
        // drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f, 0xFFFF00FF);

        MaterialNode* node = nullptr;
        if (ImGui::MenuItem("Input Action")) node = new TextureNode();
        ImGui::Separator();
        if (ImGui::MenuItem("Input Action")) node = new TextureNode();
        ImGui::Separator();

        if (node) {
            mGraph->AddNode(*node);
            createNewNode = false;

            ed::SetNodePosition(node->ID, newNodePostion);

            if (auto startPin = newNodeLinkPin) {
                auto& pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

                for (auto& pin : pins) {
                    if (startPin->Type == pin.Type) {  // CanCreateLink(startPin, &pin)
                        auto endPin = &pin;
                        if (startPin->Kind == PinKind::Input) std::swap(startPin, endPin);
                        mGraph->AddLink(GenerateLinkID(), startPin->ID, endPin->ID);
                        break;
                    }
                }
            }
        }

        ImGui::EndPopup();
    } else
        createNewNode = false;
    ImGui::PopStyleVar();
    ed::Resume();

    ed::End();

    ImGui::End();
}

void MaterialGraphPanel::OnEvent(Event& event) {}

void MaterialGraphPanel::TouchNode(NodeID id) { mNodeTouchTime[id] = mTouchTime; }

float MaterialGraphPanel::GetTouchProgress(NodeID id) {
    auto it = mNodeTouchTime.find(id);
    if (it != mNodeTouchTime.end() && it->second > 0.0f)
        return (mTouchTime - it->second) / mTouchTime;
    else
        return 0.0f;
}

void MaterialGraphPanel::UpdateTouch() {
    const auto deltaTime = ImGui::GetIO().DeltaTime;
    for (auto& entry : mNodeTouchTime) {
        if (entry.second > 0.0f) entry.second -= deltaTime;
    }
}

bool MaterialGraphPanel::Splitter(bool split_vertically, float thickness, float* size1,
                                  float* size2, float min_size1, float min_size2,
                                  float splitter_long_axis_size) {
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID("##Splitter");
    ImRect bb;
    bb.Min =
        window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size)
                                                    : ImVec2(splitter_long_axis_size, thickness),
                                   0.0f, 0.0f);
    return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2,
                            min_size1, min_size2, 0.0f);
}

ImColor MaterialGraphPanel::GetIconColor(MaterialPinType type) {
    switch (type) {
        default:
        case MaterialPinType::Unknown:
            return ImColor(255, 255, 255);
        case MaterialPinType::Bool:
            return ImColor(220, 48, 48);
        case MaterialPinType::RGB:
            return ImColor(68, 201, 156);
        case MaterialPinType::Float:
            return ImColor(147, 226, 74);
        case MaterialPinType::String:
            return ImColor(124, 21, 153);
        case MaterialPinType::BSDF:
            return ImColor(51, 150, 215);
        case MaterialPinType::Texture:
            return ImColor(218, 0, 183);
        case MaterialPinType::Flow:
            return ImColor(255, 48, 48);
    }
};

void MaterialGraphPanel::DrawPinIcon(const MaterialPin& pin, bool connected, int alpha) {
    IconType iconType;
    ImColor color = GetIconColor(pin.Type);
    color.Value.w = alpha / 255.0f;
    switch (pin.Type) {
        case MaterialPinType::Flow:
            iconType = IconType::Flow;
            break;
        case MaterialPinType::Bool:
        case MaterialPinType::Float:
        case MaterialPinType::String:
        case MaterialPinType::RGB:
        case MaterialPinType::BSDF:
        case MaterialPinType::Texture:
            iconType = IconType::Circle;
            break;
        default:
            return;
    }

    BlueprintNodeBuilder::Icon(ImVec2(mPinIconSize, mPinIconSize), iconType, connected, color,
                               ImColor(32, 32, 32, alpha));
};

void MaterialGraphPanel::ShowLeftPane(float paneWidth) {
    auto api = GlobalContext::GetDriverApi();

    auto& io = ImGui::GetIO();

    ImGui::BeginChild("Selection", ImVec2(paneWidth, 0));

    paneWidth = ImGui::GetContentRegionAvail().x;

    static bool showStyleEditor = false;
    ImGui::BeginHorizontal("Style Editor", ImVec2(paneWidth, 0));
    ImGui::Spring(0.0f, 0.0f);
    if (ImGui::Button("Zoom to Content")) ed::NavigateToContent();
    ImGui::Spring(0.0f);
    if (ImGui::Button("Show Flow")) {
        for (auto& [linkid, link] : mGraph->GetLinks()) ed::Flow(link.ID);
    }
    ImGui::Spring();
    if (ImGui::Button("Edit Style")) showStyleEditor = true;
    ImGui::EndHorizontal();

    // if (showStyleEditor) ShowStyleEditor(&showStyleEditor);

    std::vector<ed::NodeId> selectedNodes;
    std::vector<ed::LinkId> selectedLinks;
    selectedNodes.resize(ed::GetSelectedObjectCount());
    selectedLinks.resize(ed::GetSelectedObjectCount());

    int nodeCount =
        ed::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
    int linkCount =
        ed::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size()));

    selectedNodes.resize(nodeCount);
    selectedLinks.resize(linkCount);

    int saveIconWidth = EditorResource::BlueprintSaveIcon->width;
    int saveIconHeight = EditorResource::BlueprintSaveIcon->height;
    int restoreIconWidth = EditorResource::BlueprintRestoreIcon->width;
    int restoreIconHeight = EditorResource::BlueprintRestoreIcon->height;

    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetCursorScreenPos(),
        ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
        ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]),
        ImGui::GetTextLineHeight() * 0.25f);
    ImGui::Spacing();
    ImGui::SameLine();
    ImGui::TextUnformatted("Nodes");
    ImGui::Indent();
    for (auto& [nodeid, node] : mGraph->GetNodes()) {
        ImGui::PushID(node.ID);
        auto start = ImGui::GetCursorScreenPos();

        if (const auto progress = GetTouchProgress(node.ID)) {
            ImGui::GetWindowDrawList()->AddLine(
                start + ImVec2(-8, 0), start + ImVec2(-8, ImGui::GetTextLineHeight()),
                IM_COL32(255, 0, 0, 255 - (int)(255 * progress)), 4.0f);
        }

        bool isSelected = std::find(selectedNodes.begin(), selectedNodes.end(),
                                    (ed::NodeId)node.ID) != selectedNodes.end();
        if (ImGui::Selectable((node.Name + "##" + std::to_string(node.ID)).c_str(), &isSelected)) {
            if (io.KeyCtrl) {
                if (isSelected)
                    ed::SelectNode(node.ID, true);
                else
                    ed::DeselectNode(node.ID);
            } else
                ed::SelectNode(node.ID, false);

            ed::NavigateToSelection();
        }
        if (ImGui::IsItemHovered() && !node.State.empty())
            ImGui::SetTooltip("State: %s", node.State.c_str());

        auto id = std::string("(") + std::to_string(node.ID) + ")";
        auto textSize = ImGui::CalcTextSize(id.c_str(), nullptr);
        auto iconPanelPos =
            start +
            ImVec2(paneWidth - ImGui::GetStyle().FramePadding.x - ImGui::GetStyle().IndentSpacing -
                       saveIconWidth - restoreIconWidth - ImGui::GetStyle().ItemInnerSpacing.x * 1,
                   (ImGui::GetTextLineHeight() - saveIconHeight) / 2);
        ImGui::GetWindowDrawList()->AddText(
            ImVec2(iconPanelPos.x - textSize.x - ImGui::GetStyle().ItemInnerSpacing.x, start.y),
            IM_COL32(255, 255, 255, 255), id.c_str(), nullptr);

        auto drawList = ImGui::GetWindowDrawList();
        ImGui::SetCursorScreenPos(iconPanelPos);
        ImGui::SetItemAllowOverlap();
        if (node.SavedState.empty()) {
            if (ImGui::InvisibleButton("save", ImVec2((float)saveIconWidth, (float)saveIconHeight)))
                node.SavedState = node.State;

            if (ImGui::IsItemActive())
                drawList->AddImage((ImTextureID)api->GetTextueID(EditorResource::BlueprintSaveIcon),
                                   ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0),
                                   ImVec2(1, 1), IM_COL32(255, 255, 255, 96));
            else if (ImGui::IsItemHovered())
                drawList->AddImage((ImTextureID)api->GetTextueID(EditorResource::BlueprintSaveIcon),
                                   ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0),
                                   ImVec2(1, 1), IM_COL32(255, 255, 255, 255));
            else
                drawList->AddImage((ImTextureID)api->GetTextueID(EditorResource::BlueprintSaveIcon),
                                   ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0),
                                   ImVec2(1, 1), IM_COL32(255, 255, 255, 160));
        } else {
            ImGui::Dummy(ImVec2((float)saveIconWidth, (float)saveIconHeight));
            drawList->AddImage((ImTextureID)api->GetTextueID(EditorResource::BlueprintSaveIcon),
                               ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0),
                               ImVec2(1, 1), IM_COL32(255, 255, 255, 32));
        }

        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::SetItemAllowOverlap();
        if (!node.SavedState.empty()) {
            if (ImGui::InvisibleButton("restore",
                                       ImVec2((float)restoreIconWidth, (float)restoreIconHeight))) {
                node.State = node.SavedState;
                ed::RestoreNodeState(node.ID);
                node.SavedState.clear();
            }

            if (ImGui::IsItemActive())
                drawList->AddImage(
                    (ImTextureID)api->GetTextueID(EditorResource::BlueprintRestoreIcon),
                    ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1),
                    IM_COL32(255, 255, 255, 96));
            else if (ImGui::IsItemHovered())
                drawList->AddImage(
                    (ImTextureID)api->GetTextueID(EditorResource::BlueprintRestoreIcon),
                    ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1),
                    IM_COL32(255, 255, 255, 255));
            else
                drawList->AddImage(
                    (ImTextureID)api->GetTextueID(EditorResource::BlueprintRestoreIcon),
                    ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1),
                    IM_COL32(255, 255, 255, 160));
        } else {
            ImGui::Dummy(ImVec2((float)restoreIconWidth, (float)restoreIconHeight));
            drawList->AddImage((ImTextureID)api->GetTextueID(EditorResource::BlueprintRestoreIcon),
                               ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0),
                               ImVec2(1, 1), IM_COL32(255, 255, 255, 32));
        }

        ImGui::SameLine(0, 0);
        ImGui::SetItemAllowOverlap();
        ImGui::Dummy(ImVec2(0, (float)restoreIconHeight));

        ImGui::PopID();
    }
    ImGui::Unindent();

    static int changeCount = 0;

    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetCursorScreenPos(),
        ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
        ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]),
        ImGui::GetTextLineHeight() * 0.25f);
    ImGui::Spacing();
    ImGui::SameLine();
    ImGui::TextUnformatted("Selection");

    ImGui::BeginHorizontal("Selection Stats", ImVec2(paneWidth, 0));
    ImGui::Text("Changed %d time%s", changeCount, changeCount > 1 ? "s" : "");
    ImGui::Spring();
    if (ImGui::Button("Deselect All")) ed::ClearSelection();
    ImGui::EndHorizontal();
    ImGui::Indent();
    for (int i = 0; i < nodeCount; ++i) ImGui::Text("Node (%p)", selectedNodes[i].AsPointer());
    for (int i = 0; i < linkCount; ++i) ImGui::Text("Link (%p)", selectedLinks[i].AsPointer());
    ImGui::Unindent();

    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
        for (auto& [linkid, link] : mGraph->GetLinks()) ed::Flow(link.ID);

    if (ed::HasSelectionChanged()) ++changeCount;

    ImGui::EndChild();
}

}  // namespace Ethereal
