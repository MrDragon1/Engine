#include "MaterialGraphPanel.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include <imgui_node_editor/imgui_node_editor.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "Base/GlobalContext.h"

#include "Utils/XmlParser.h"
#include "Core/Material/MaterialBase/Document.h"
#include "Core/Material/MaterialBase/Value.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {

MaterialGraphPanel::MaterialGraphPanel() {
    mEditor = ed::CreateEditor();
    ed::SetCurrentEditor(mEditor);
    mMaterial = MaterialManager::LoadMaterial("assets/materials/disney_material.mtlx");
    // mMaterial = MaterialManager::LoadMaterial("assets/materials/material.mtlx");
    PrepareNodeMenuInfo();

    SetGraph(mMaterial->GenerateUIGraph());
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

    auto& io = ImGui::GetIO();

    auto pos = ImGui::GetCursorScreenPos();
    ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

    ed::SetCurrentEditor(mEditor);

    static ed::NodeId contextNodeId = 0;
    static ed::LinkId contextLinkId = 0;
    static ed::PinId contextPinId = 0;
    static bool createNewNode = false;
    static MaterialPinPtr newNodeLinkPin = nullptr;
    static MaterialPinPtr newLinkPin = nullptr;

    static float leftPaneWidth = 400.0f;
    static float rightPaneWidth = 800.0f;
    Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);

    ShowLeftPanel(leftPaneWidth - 4.0f);

    ImGui::SameLine(0.0f, 12.0f);
    ed::Begin("Node editor");
    {
        auto cursorTopLeft = ImGui::GetCursorScreenPos();
        auto api = GlobalContext::GetDriverApi();
        auto bg = EditorResource::MaterialGraphHeaderBG;
        BlueprintNodeBuilder builder((ImTextureID)api->GetTextueID(bg), bg->width, bg->height);

        for (auto& [id, node] : mCurrentGraph->GetNodes()) {
            if (node->mType != NodeType::Blueprint && node->mType != NodeType::Simple) continue;

            const auto isSimple = node->mType == NodeType::Simple;

            builder.Begin(node->mID);
            if (!isSimple) {
                builder.Header(ImVec4(node->mColor.x, node->mColor.y, node->mColor.z, 1.0));
                ImGui::Spring(0);
                ImGui::TextUnformatted(node->mName.c_str());
                ImGui::Spring(1);
                ImGui::Dummy(ImVec2(0, 28));
                ImGui::Spring(0);
                builder.EndHeader();
            }

            for (auto& [_, input] : node->mInputs) {
                auto alpha = ImGui::GetStyle().Alpha;
                if (newLinkPin && false && input != newLinkPin) alpha = alpha * (48.0f / 255.0f);

                builder.Input(input->mID);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                DrawPinIcon(input, mCurrentGraph->IsPinLinked(input->mID), (int)(alpha * 255));
                ImGui::Spring(0);
                if (!input->mName.empty()) {
                    ImGui::TextUnformatted(input->mName.c_str());
                    ImGui::Spring(0);
                }
                ImGui::PopStyleVar();
                builder.EndInput();
            }

            if (isSimple) {
                builder.Middle();

                ImGui::Spring(1, 0);
                ImGui::TextUnformatted(node->mName.c_str());
                ImGui::Spring(1, 0);
            }

            for (auto& [_, output] : node->mOutputs) {
                if (!isSimple && false) continue;

                auto alpha = ImGui::GetStyle().Alpha;
                if (newLinkPin && !true && output != newLinkPin) alpha = alpha * (48.0f / 255.0f);

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                builder.Output(output->mID);

                if (output->mType == MaterialPinType::String) {
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

                if (!output->mName.empty()) {
                    ImGui::Spring(0);
                    ImGui::TextUnformatted(output->mName.c_str());
                }
                ImGui::Spring(0);
                DrawPinIcon(output, mCurrentGraph->IsPinLinked(output->mID), (int)(alpha * 255));

                ImGui::PopStyleVar();
                builder.EndOutput();
            }

            builder.End();
        }

        for (auto& [id, link] : mCurrentGraph->GetLinks())
            ed::Link(link->mID, link->mSrcID, link->mDstID);

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
                    auto startPin = mCurrentGraph->GetPin((PinID)startPinId);
                    auto endPin = mCurrentGraph->GetPin((PinID)endPinId);

                    newLinkPin = startPin ? startPin : endPin;

                    if (startPin->mKind == PinKind::Input) {
                        std::swap(startPin, endPin);
                        std::swap(startPinId, endPinId);
                    }

                    if (startPin && endPin) {
                        if (endPin == startPin) {
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        } else if (endPin->mKind == startPin->mKind) {
                            showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        // else if (endPin->Node == startPin->Node)
                        //{
                        //     showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                        //     ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                        // }
                        else if (endPin->mType != startPin->mType) {
                            showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                        } else {
                            showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                                AddLink(mCurrentGraph->GenerateID(), (PinID)startPinId,
                                        (PinID)endPinId);
                                // s_Links.back().Color = GetIconColor(startPin->Type);
                            }
                        }
                    }
                }

                ed::PinId pinId = 0;
                if (ed::QueryNewNode(&pinId)) {
                    newLinkPin = mCurrentGraph->GetPin((PinID)pinId);
                    if (newLinkPin) showLabel("+ Create Node", ImColor(32, 45, 32, 180));

                    if (ed::AcceptNewItem()) {
                        createNewNode = true;
                        newNodeLinkPin = mCurrentGraph->GetPin((PinID)pinId);
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
                        RemoveLink((LinkID)linkId);
                    }
                }

                ed::NodeId nodeId = 0;
                while (ed::QueryDeletedNode(&nodeId)) {
                    if (ed::AcceptDeletedItem()) {
                        RemoveNode((NodeID)nodeId);
                    }
                }
            }
            ed::EndDelete();
        }

        ImGui::SetCursorScreenPos(cursorTopLeft);
    }

    if (mAutoLayout) ResetLayout();

    ed::NodeId doubleClickNodeId = ed::GetDoubleClickedNode();
    MaterialNodePtr doubleClickNode = mCurrentGraph->GetNode((size_t)doubleClickNodeId);
    if (doubleClickNode) {
        ElementPtr source = doubleClickNode->mSource;
        if (source && source->Is(MaterialElementType::NODEGRAPH)) {
            SetGraph(
                mMaterial->GetDocument()->GenerateUIGraphFromNodeGraph(source.As<NodeGraph>()));
        } else if (source && source->Is(MaterialElementType::NODEINSTANCE)) {
            NodeInstancePtr ni = source.As<NodeInstance>();
            NodeImplPtr impl = ni->GetNodeImpl();

            // if the node is implement by a shader file, we will not show the subgraph
            if (impl && impl->IsNodeGraph()) {
                SetGraph(
                    mMaterial->GetDocument()->GenerateUIGraphFromNodeGraph(impl->GetNodeGraph()));
            }
        } else {
            std::cout << "empty subgraph select" << std::endl;
        }
    }

    // deal with popup
    auto openPopupPosition = ImGui::GetMousePos();
    ed::Suspend();

    if (ed::ShowBackgroundContextMenu()) {
        ImGui::OpenPopup("Create New Node");
        newNodeLinkPin = nullptr;
    }
    ed::Resume();

    ed::Suspend();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("Create New Node")) {
        auto newNodePosition = openPopupPosition;
        // ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

        // auto drawList = ImGui::GetWindowDrawList();
        // drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f, 0xFFFF00FF);

        MaterialNodePtr node = CreateNodeMenu();

        if (node) {
            AddNode(node);
            createNewNode = false;
            node->mPosition = Vector2(newNodePosition.x, newNodePosition.y);
            ed::SetNodePosition(node->mID, newNodePosition);

            if (auto startPin = newNodeLinkPin) {
                auto& pins = startPin->mKind == PinKind::Input ? node->mOutputs : node->mInputs;

                for (auto& [_, pin] : pins) {
                    if (startPin->mType == pin->mType) {  // CanCreateLink(startPin, &pin)
                        auto endPin = pin;
                        if (startPin->mKind == PinKind::Input) std::swap(startPin, endPin);
                        AddLink(mCurrentGraph->GenerateID(), startPin->mID, endPin->mID);
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

void MaterialGraphPanel::SetGraph(MaterialGraphPtr graph) {
    if (mCurrentGraph) {
        mGraphStack.push(mCurrentGraph);
    }
    mCurrentGraph = graph;
    mAutoLayout = true;
}

void MaterialGraphPanel::PopGraph() {
    if (mGraphStack.empty()) {
        ET_CORE_WARN("Graph stack is empty!");
        return;
    }
    mCurrentGraph = mGraphStack.top();
    mGraphStack.pop();
    mAutoLayout = true;
}

void MaterialGraphPanel::AddNode(MaterialNodePtr node) {
    // More operation refer to CreateNodeMenu()
    mCurrentGraph->AddNode(node);
}

void MaterialGraphPanel::RemoveNode(NodeID nodeId) {
    auto node = mCurrentGraph->GetNode(nodeId);

    if (node->mSource->Is(MaterialElementType::INPUT)) {
        node->mSource.As<NodeInput>()->Disconnect();
    } else if (node->mSource->Is(MaterialElementType::OUTPUT)) {
        node->mSource.As<NodeOutput>()->Disconnect();
    } else if (!node->mSource->Is(MaterialElementType::DOCUMENT)) {
        for (auto& input : node->mSource->GetInputs()) {
            input->Disconnect();
        }
        for (auto& output : node->mSource->GetOutputs()) {
            output->Disconnect();
        }
    }
    auto doc = mMaterial->GetDocument();
    doc->RemoveNodeInstance(node->mSource->GetName());

    mCurrentGraph->RemoveNode((NodeID)nodeId);
}

void MaterialGraphPanel::AddLink(LinkID id, PinID src, PinID dst) {
    mCurrentGraph->AddLink(id, src, dst);
}

void MaterialGraphPanel::RemoveLink(LinkID linkId) { mCurrentGraph->RemoveLink((LinkID)linkId); }

void MaterialGraphPanel::ShowLeftPanel(float width) {
    static float previewPanelHeight = 200.0f;
    static float inspectorPanelHeight = 200.0f;
    ImGui::BeginChild("left panel", ImVec2(width, 0));

    Splitter(false, 4.0f, &previewPanelHeight, &inspectorPanelHeight, 50.0f, 50.0f);

    width = ImGui::GetContentRegionAvail().x;
    ImVec2 windowPos = ImGui::GetWindowPos();
    ShowPreviewPanel(width, previewPanelHeight);
    ShowUINodeInspectorPanel(width, inspectorPanelHeight);

    ImGui::EndChild();

    ImVec2 screenSize = ImVec2(width, previewPanelHeight);
    // HandleInputs(windowPos, screenSize[0], screenSize[1]);
}

void MaterialGraphPanel::ShowUINodeInspectorPanel(float panelWidth, float panelHeight) {
    std::vector<ed::NodeId> selectedNodeIds;
    std::vector<ed::LinkId> selectedLinkIds;
    selectedNodeIds.resize(ed::GetSelectedObjectCount());
    selectedLinkIds.resize(ed::GetSelectedObjectCount());

    int nodeCount =
        ed::GetSelectedNodes(selectedNodeIds.data(), static_cast<int>(selectedNodeIds.size()));
    int linkCount =
        ed::GetSelectedLinks(selectedLinkIds.data(), static_cast<int>(selectedLinkIds.size()));

    selectedNodeIds.resize(nodeCount);
    selectedLinkIds.resize(linkCount);

    std::vector<MaterialNodePtr> selectedNodes;
    std::vector<MaterialLinkPtr> selectedLinks;
    for (auto& nodeid : selectedNodeIds) {
        for (auto& [id, node] : mCurrentGraph->GetNodes()) {
            if (node->mID == (NodeID)nodeid) {
                selectedNodes.push_back(node);
                break;
            }
        }
    }
    for (auto& linkid : selectedLinkIds) {
        for (auto& [id, link] : mCurrentGraph->GetLinks()) {
            if (link->mID == (NodeID)linkid) {
                selectedLinks.push_back(link);
                break;
            }
        }
    }

    ImGui::BeginHorizontal("Style Editor", ImVec2(panelWidth, 0));
    ImGui::Spring(0.0f, 0.0f);
    if (ImGui::Button("Zoom to Content")) ed::NavigateToContent();
    if (ImGui::Button("Back")) {
        PopGraph();
    }
    if (ImGui::Button("Reset Layout")) mAutoLayout = true;
    if (ImGui::Button("Compile")) Compile();

    ImGui::EndHorizontal();

    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetCursorScreenPos(),
        ImGui::GetCursorScreenPos() + ImVec2(panelWidth, ImGui::GetTextLineHeight()),
        ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]),
        ImGui::GetTextLineHeight() * 0.25f);
    ImGui::Spacing();
    ImGui::SameLine();
    ImGui::TextUnformatted("Inspector");

    for (auto& node : selectedNodes) {
        ImGui::Text("Name: %s", node->mName.c_str());
        ImGui::Text("Type: %s", node->mSource->GetType().c_str());
        ImGui::Separator();
        ImGui::Text("Inputs:");
        ImGui::Indent();
        for (auto& [name, input] : node->GetInputs()) {
            float prevpos = ImGui::GetCursorScreenPos().x;
            ImGui::Text("%s: [%s] ", input->mName.c_str(), PinTypeToString(input->mType).c_str());
            ImGui::SameLine(panelWidth / 2, 0);
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 5);
            DrawPinController(input);
            ImGui::PopItemWidth();
        }
        ImGui::Unindent();

        ImGui::Text("Outputs:");
        ImGui::Indent();
        for (auto& [name, output] : node->GetOutputs()) {
            ImGui::Text("%s: [%s] ", output->mName.c_str(), PinTypeToString(output->mType).c_str());
        }
        ImGui::Unindent();
        ImGui::Separator();
    }
}

void MaterialGraphPanel::ShowPreviewPanel(float panelWidth, float panelHeight) {
    ImGui::BeginHorizontal("Preview", ImVec2(panelWidth, panelHeight));
    auto& renderSystem = GlobalContext::GetRenderSystem();
    ImGui::Spring(0.0f, 0.0f);
    auto& io = ImGui::GetIO();

    auto img = renderSystem.DrawMaterialPreview(mMaterial, (size_t)panelWidth, (size_t)panelHeight);

    ImGui::Image(reinterpret_cast<void*>(img), ImVec2{panelWidth, panelHeight}, ImVec2{0, 1},
                 ImVec2{1, 0});
    // ImGui::Text("Preview Image Here");

    ImGui::EndHorizontal();
}

MaterialNodePtr MaterialGraphPanel::CreateNodeMenu() {
    MaterialNodePtr nodeUI = nullptr;
    if (!mMaterial) return nodeUI;

    for (auto trunk : mBuildinNodeDef) {
        ImGui::SetNextWindowSizeConstraints(ImVec2(100, 10), ImVec2(250, 300));
        if (ImGui::BeginMenu(trunk.first.c_str())) {
            for (auto& nodeDef : trunk.second) {
                std::string name = nodeDef->GetName();
                if (ImGui::MenuItem(name.substr(3, name.length()).c_str())) {
                    if (mCurrentGraph->GetSource()->Is(MaterialElementType::DOCUMENT)) {
                        DocumentPtr doc = mCurrentGraph->GetSource().As<Document>();
                        NodeInstancePtr node =
                            doc->AddNodeInstnce(nodeDef, name.substr(3, name.length()));
                        nodeUI = MaterialNodePtr::Create(node, mCurrentGraph);
                    } else if (mCurrentGraph->GetSource()->Is(MaterialElementType::NODEGRAPH)) {
                        NodeGraphPtr graph = mCurrentGraph->GetSource().As<NodeGraph>();
                        NodeInstancePtr node =
                            graph->AddNodeInstnce(nodeDef, name.substr(3, name.length()));
                        nodeUI = MaterialNodePtr::Create(node, mCurrentGraph);
                    }
                }
                ImGui::Separator();
            }
            ImGui::EndMenu();
        }
    }
    return nodeUI;
}

void MaterialGraphPanel::PrepareNodeMenuInfo() {
    mBuildinNodeDef.clear();
    const std::string EXTRA_GROUP_NAME = "extra";
    for (auto& [name, nodeDefs] : mMaterial->GetDocument()->GetAllNodeDefines()) {
        for (auto& nodeDef : nodeDefs) {
            std::string group = nodeDef->GetNodeGroup();
            if (group.empty()) {
                group = EXTRA_GROUP_NAME;
            }
            if (mBuildinNodeDef.find(group) == mBuildinNodeDef.end()) {
                mBuildinNodeDef[group] = std::vector<NodeDefinePtr>();
            }
            mBuildinNodeDef[group].push_back(nodeDef);
        }
    }
}

void MaterialGraphPanel::DrawPinController(MaterialPinPtr pin) {
#define BEGIN_UPDATE() {
#define ENG_UPDATE() \
    break;           \
    }
    if (!pin->mSource || !pin->mSource->Is(MaterialElementType::INPUT)) {
        ImGui::Text("");
        return;
    }
    NodeInputPtr input = pin->mSource.As<NodeInput>();
    if (input->GetConnector()) {
        ImGui::Text("");
        return;
    }

    std::string dummyname = ("##" + pin->mName);
    switch (pin->mType) {
        case MaterialPinType::Bool:
            BEGIN_UPDATE();
            ImGui::Checkbox(dummyname.c_str(), input->GetValue()->GetPtr<bool>());
            ENG_UPDATE();
        case MaterialPinType::Int:
            BEGIN_UPDATE();
            ImGui::DragInt(dummyname.c_str(), input->GetValue()->GetPtr<int>(), 1.0f);
            ENG_UPDATE();
        case MaterialPinType::Float:
            BEGIN_UPDATE();
            ImGui::DragFloat(dummyname.c_str(), input->GetValue()->GetPtr<float>(), 0.01f);
            ENG_UPDATE();
        case MaterialPinType::Float2:
            BEGIN_UPDATE();
            ImGui::DragFloat2(dummyname.c_str(), (float*)input->GetValue()->GetPtr<Vector2>(),
                              0.01f);
            ENG_UPDATE();
        case MaterialPinType::Float3:
            BEGIN_UPDATE();
            ImGui::DragFloat3(dummyname.c_str(), (float*)input->GetValue()->GetPtr<Vector3>(),
                              0.01f);
            ENG_UPDATE();
        case MaterialPinType::Float4:
            BEGIN_UPDATE();
            ImGui::DragFloat4(dummyname.c_str(), (float*)input->GetValue()->GetPtr<Vector4>(),
                              0.01f);
            ENG_UPDATE();
        case MaterialPinType::Color3:
            BEGIN_UPDATE();
            ImGui::ColorEdit3(dummyname.c_str(), (float*)input->GetValue()->GetPtr<Color3>());
            ENG_UPDATE();
        case MaterialPinType::Color4:
            BEGIN_UPDATE();
            ImGui::ColorEdit4(dummyname.c_str(), (float*)input->GetValue()->GetPtr<Color4>());
            ENG_UPDATE();
        case MaterialPinType::String:
            BEGIN_UPDATE();
            ImGui::InputText(dummyname.c_str(), input->GetValue()->GetPtr<string>());
            ENG_UPDATE();
        default:
            ImGui::Text("Unknown Controller");
    }
}

void MaterialGraphPanel::Compile() {
    mMaterial->Compile();

    // Regenerate UI graph
    while (mGraphStack.size() > 1) PopGraph();
    SetGraph(mMaterial->GenerateUIGraph());
}

void MaterialGraphPanel::ResetLayout() {
    mAutoLayout = false;
    for (auto& [id, node] : mCurrentGraph->GetNodes()) {
        node->mSize.x = ed::GetNodeSize(node->mID).x;
        node->mSize.y = ed::GetNodeSize(node->mID).y;
    }

    mCurrentGraph->UpdateLayout();

    for (auto& [id, node] : mCurrentGraph->GetNodes()) {
        ed::SetNodePosition(node->mID, ImVec2(node->mPosition.x, node->mPosition.y));
    }
    ed::NavigateToContent();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////      Utils      ///////////////////////////////
//////////////////////////////////////////////////////////////////////////

MaterialPinType MaterialGraphPanel::GetPinType(const string& type) {
    if (type == "boolean") return MaterialPinType::Bool;
    if (type == "integer") return MaterialPinType::Int;
    if (type == "float") return MaterialPinType::Float;
    if (type == "float2") return MaterialPinType::Float2;
    if (type == "float3") return MaterialPinType::Float3;
    if (type == "float4") return MaterialPinType::Float4;
    if (type == "color3") return MaterialPinType::Color3;
    if (type == "color4") return MaterialPinType::Color4;
    if (type == "string") return MaterialPinType::String;
    return MaterialPinType::Flow;
}

const string MaterialGraphPanel::PinTypeToString(MaterialPinType type) {
    if (type == MaterialPinType::Bool) return "boolean";
    if (type == MaterialPinType::Int) return "integer";
    if (type == MaterialPinType::Float) return "float";
    if (type == MaterialPinType::Float2) return "float2";
    if (type == MaterialPinType::Float3) return "float3";
    if (type == MaterialPinType::Float4) return "float4";
    if (type == MaterialPinType::Color3) return "color3";
    if (type == MaterialPinType::Color4) return "color4";
    if (type == MaterialPinType::String) return "string";
    return "none";
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
        case MaterialPinType::Flow:
            return ImColor(255, 255, 255);
        case MaterialPinType::Bool:
            return ImColor(255, 0, 255);
        case MaterialPinType::Int:
            return ImColor(50, 100, 255);
        case MaterialPinType::Float:
            return ImColor(178, 34, 34);
        case MaterialPinType::Float2:
            return ImColor(255, 10, 28);
        case MaterialPinType::Float3:
            return ImColor(100, 255, 100);
        case MaterialPinType::Float4:
            return ImColor(0, 255, 0);
        case MaterialPinType::Color3:
            return ImColor(100, 0, 100);
        case MaterialPinType::Color4:
            return ImColor(0, 100, 100);
        case MaterialPinType::String:
            return ImColor(100, 100, 50);
        case MaterialPinType::Object:
            return ImColor(255, 184, 28);
    }
};

void MaterialGraphPanel::DrawPinIcon(MaterialPinPtr pin, bool connected, int alpha) {
    IconType iconType;
    ImColor color = GetIconColor(pin->mType);
    color.Value.w = alpha / 255.0f;
    switch (pin->mType) {
        case MaterialPinType::Flow:
            iconType = IconType::Flow;
            break;
        case MaterialPinType::Bool:
        case MaterialPinType::Int:
        case MaterialPinType::Float:
        case MaterialPinType::Float2:
        case MaterialPinType::Float3:
        case MaterialPinType::Float4:
        case MaterialPinType::Color3:
        case MaterialPinType::Color4:
        case MaterialPinType::String:
            iconType = IconType::Circle;
            break;
        case MaterialPinType::Object:
            iconType = IconType::Diamond;
            break;
        default:
            iconType = IconType::RoundSquare;
    }

    BlueprintNodeBuilder::Icon(ImVec2(mPinIconSize, mPinIconSize), iconType, connected, color,
                               ImColor(32, 32, 32, alpha));
};

}  // namespace Ethereal
