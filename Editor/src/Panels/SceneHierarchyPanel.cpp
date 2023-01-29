#include "SceneHierarchyPanel.h"

#include "Core/Asset/AssetManager.h"
#include "Base/ImGui/UI.h"
#include "Core/Editor/EditorResource.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <filesystem>

namespace Ethereal
{
    SelectionContext SceneHierarchyPanel::s_ActiveSelectionContext = SelectionContext::Scene;

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context, SelectionContext selectionContext)
        : m_Context(context), m_SelectionContext(selectionContext) {}

    void SceneHierarchyPanel::SetSceneContext(const Ref<Scene>& scene) { m_Context = scene; }

    void SceneHierarchyPanel::OnEvent(Event& event) {}

    void SceneHierarchyPanel::OnImGuiRender(bool& isOpen) {
        s_ActiveSelectionContext = m_SelectionContext;

        ShowHierarchy();
        ShowInspector();
    }

    void SceneHierarchyPanel::DrawEntityCreateMenu(Entity parent) {
        Entity newEntity;
        if (ImGui::MenuItem("Create Empty Entity")) m_Context->CreateEntity("Empty Entity");
        if (ImGui::BeginMenu("3D Object")) {
            if (ImGui::MenuItem("Cube")) newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CUBE);
            if (ImGui::MenuItem("Sphere")) newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_SPHERE);
            if (ImGui::MenuItem("Cylinder")) newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CYLINDER);
            if (ImGui::MenuItem("Cone")) newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CONE);
            if (ImGui::MenuItem("Torus")) newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_TORUS);
            if (ImGui::MenuItem("Plane")) newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_PLANE);
            if (ImGui::MenuItem("Quad")) newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_QUAD);
            if (ImGui::MenuItem("Monkey")) newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_MONKEY);
            ImGui::EndMenu();
        }
        if (newEntity) {
            //            if (parent)
            //                m_Context->ParentEntity(newEntity, parent);

            SelectionManager::DeselectAll();
            SelectionManager::Select(s_ActiveSelectionContext, newEntity.GetUUID());
        }

        if (ImGui::MenuItem("Material")) {
            MaterialDesc desc;
            desc.Name = "M_Default";
            AssetManager::CreateAsset_Ref("M_Default", (Project::GetAssetDirectory() / "materials").string(), desc);
        }
    }

    enum class VectorAxis
    {
        X = BIT(0),
        Y = BIT(1),
        Z = BIT(2),
        W = BIT(3)
    };

    template <typename TComponent, typename UIFunction>
    void SceneHierarchyPanel::DrawComponent(const std::string& name, UIFunction uiFunction, Ref<Texture2D> icon) {
        bool shouldDraw = true;

        auto& entities = SelectionManager::GetSelections(s_ActiveSelectionContext);
        for (const auto& entityID : entities) {
            Entity entity = m_Context->GetEntityWithUUID(entityID);
            if (!entity.HasComponent<TComponent>()) {
                shouldDraw = false;
                break;
            }
        }

        if (!shouldDraw) return;

        if (icon == nullptr) icon = EditorResource::AssetIcon;

        // NOTE(Peter):
        //	This fixes an issue where the first "+" button would display the "Remove" buttons for ALL components on an Entity.
        //	This is due to ImGui::TreeNodeEx only pushing the id for it's children if it's actually open
        ImGui::PushID((void*)typeid(TComponent).hash_code());

        bool open = UI::ComponentHeader(name.c_str(), ImGuiTreeNodeFlags_None);
        bool right_clicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
        float lineHeight = ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y;

        bool resetValues = false;
        bool removeComponent = false;

        if (open) {
            Entity entity = m_Context->GetEntityWithUUID(entities[0]);
            TComponent& firstComponent = entity.GetComponent<TComponent>();
            const bool isMultiEdit = entities.size() > 1;
            uiFunction(firstComponent, entities, isMultiEdit, name);
        }
        ImGui::PopID();
    }

    void SceneHierarchyPanel::ShowInspector() {
        UI::ScopedStyleStack style(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 4.0f), ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::Begin("Inspector", nullptr);

        const std::vector<UUID> entities = SelectionManager::GetSelections(s_ActiveSelectionContext);
        if (!entities.empty()) {
            auto entity = m_Context->GetEntityWithUUID(entities[0]);

            UI::ShiftCursorX(10.0f);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 3.0f);
            ImGui::InputText("##InspectorTag", (char *)entity.GetComponent<BasicPropertyComponent>().Tag.c_str(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
            ImGui::SameLine(ImGui::GetWindowWidth() - 30.0f);
            auto icon = EditorResource::PlusIcon;
            if (UI::ImageButton("InspectorPlusIcon", (ImTextureID)icon->GetRendererID(), ImVec2(20.0f, 20.0f))) {
                ImGui::OpenPopup("InspectorADDComponent");
            }

            if (ImGui::BeginPopup("InspectorADDComponent"))
            {
                if(!entity.HasComponent<MeshComponent>() && ImGui::MenuItem("Mesh")) entity.AddComponent<MeshComponent>();
                if(!entity.HasComponent<StaticMeshComponent>() && ImGui::MenuItem("StaticMesh")) entity.AddComponent<StaticMeshComponent>();

                ImGui::EndPopup();
            }

            DrawComponent<TransformComponent>("Transform", [&](TransformComponent& firstComponent, const std::vector<UUID>& entities, const bool isMultiEdit, const std::string& type_name) {
                    // Not support multi edit for now
                    Entity entity = m_Context->GetEntityWithUUID(entities[0]);
                    auto& component = entity.GetComponent<TransformComponent>();

                    UI::ScopedStyleStack style(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f), ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
                    ImGui::BeginTable("transformComponent", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV);
                    ImGui::TableSetupColumn("transform_label_column", 0, 80.0f);
                    ImGui::TableSetupColumn("transform_value_column", ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoClip,
                                            ImGui::GetContentRegionAvail().x - 80.0f);  // Must set Sizing Policy like ImGuiTableFlags_SizingFixedFit

                    ImGui::TableNextRow();
                    UI::DrawVec3Slider("Translation", component.Position);

                    ImGui::TableNextRow();
                    Vector3 rotation = Math::Degrees(component.Rotation);
                    UI::DrawVec3Slider("Rotation", rotation);
                    component.Rotation = Math::Radians(rotation);

                    ImGui::TableNextRow();
                    UI::DrawVec3Slider("Scale", component.Scale, Vector3(1.0f)); // TODO: load default scale as resetvalue

                    ImGui::EndTable();
                });

            DrawComponent<MeshComponent>("Mesh", [&](MeshComponent& firstComponent, const std::vector<UUID>& entities, const bool isMultiEdit, const std::string& type_name) {
                static float spacing = 10.0f;
                UI::ScopedStyleStack style(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, spacing), ImGuiStyleVar_CellPadding, ImVec2(4.0f, spacing));
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + spacing / 2.0f);

                ImGui::Button("A", {1.0f, 1.0f});
                ImGui::SameLine();
                UI::DragDropBar(("##" + type_name).c_str(), type_name.c_str(), firstComponent.MeshHandle, AssetType::Mesh);

                UI::ListHeader("Materials", firstComponent.MaterialTableRaw.Materials, AssetType::Material);
                firstComponent.PostLoad();
            });

            DrawComponent<StaticMeshComponent>("Static Mesh", [&](StaticMeshComponent& firstComponent, const std::vector<UUID>& entities, const bool isMultiEdit, const std::string& type_name) {
                    static float spacing = 10.0f;
                    UI::ScopedStyleStack style(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, spacing), ImGuiStyleVar_CellPadding, ImVec2(4.0f, spacing));
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + spacing / 2.0f);

                    // FixMe: wired bug, if we don't draw the button with size > 0, the label in DragDropBar won't be aligned with
                    //  the text in InputText.
                    ImGui::Button("A", {1.0f, 1.0f});
                    ImGui::SameLine();
                    UI::DragDropBar(("##" + type_name).c_str(), type_name.c_str(), firstComponent.StaticMeshHandle, AssetType::StaticMesh);

                    UI::ListHeader("Materials", firstComponent.MaterialTableRaw.Materials, AssetType::Material);
                    firstComponent.PostLoad();
                });
        }

        ImGui::End();
    }

    void SceneHierarchyPanel::ShowHierarchy() {
        ImGui::Begin("Hierarchy");

        static ImGuiTableFlags flags = ImGuiTableFlags_BordersV
                                       | ImGuiTableFlags_BordersOuterH
                                       | ImGuiTableFlags_Resizable
                                       | ImGuiTableFlags_RowBg
                                       | ImGuiTableFlags_NoBordersInBody
//                                       | ImGuiTableFlags_ScrollX // this will create child window which will cause the empty area not able to be clicked
                                       // | ImGuiTableFlags_SizingFixedFit
                                       ;

        if (ImGui::BeginTable("HierarchyTable", 3, flags))
        {
            // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
            ImGui::TableSetupColumn("##Visible");
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Type");


            ImGui::TableHeadersRow();

            struct EntityNode
            {
                bool Visible;
                const char*     Name;
                const char*     Type;
                uint64_t           ChildIdx;
                std::vector<uint64_t>  ChildList;

                static void DisplayNode(const EntityNode* node, const EntityNode* all_nodes, Scene* context )
                {
                    const float text_width = ImGui::CalcTextSize("A").x;
                    const float text_height = ImGui::GetTextLineHeight();

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    auto icon = node->Visible ? EditorResource::EyeIcon : EditorResource::EyeCrossIcon;
                    {
//                        UI::ShiftCursorY(2.0f);
                        UI::ScopedColorStack style(ImGuiCol_Border, IM_COL32(0, 0, 0, 0),
                                               ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
                        if(UI::ImageButton(("VisibleIcon" + std::to_string(node->ChildIdx)).c_str(), (ImTextureID)icon->GetRendererID(), ImVec2(text_height, text_height), ImU32(IM_COL32(196, 196, 196, 255)),ImU32(IM_COL32(255, 255, 255, 255)),ImU32(IM_COL32(255, 255, 255, 255)))){
                            Entity entity = context->GetEntityWithUUID(node->ChildIdx);
                            entity.ChangeVisible();
                        }
//                        UI::ShiftCursorY(-2.0f);
                    }


                    ImGui::TableNextColumn();
                    const bool is_selected = SelectionManager::IsSelected(s_ActiveSelectionContext, node->ChildIdx);
                    const bool is_folder = !node->ChildList.empty();
                    ImGuiTreeNodeFlags  node_flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow;
                    if (is_selected)
                        node_flags |= ImGuiTreeNodeFlags_Selected;

                    ImGui::TreeNodeEx(node->Name, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | node_flags );
                    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                    {
                        SelectionManager::UniqueSelect(s_ActiveSelectionContext, node->ChildIdx);
                    }
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(node->Type);

                }
            };
            std::vector<EntityNode> all_nodes;

            if(m_Context){
                m_Context->m_Registry.each([&](auto entityID) {
                    Entity entity{entityID, m_Context.Raw()};
                    EntityNode node{entity.IsVisible(), entity.GetName().c_str(), "Entity", entity.GetUUID(), {}};
                    all_nodes.push_back(node);
                });
            }

            for(auto& node:all_nodes){
                EntityNode::DisplayNode(&node, all_nodes.data(), m_Context.Raw());
            }

            ImGui::EndTable();
        }

        if(ImGui::BeginPopupContextWindow("HierarchyPopup", ImGuiMouseButton_Right, false))
        {
            DrawEntityCreateMenu();
            ImGui::EndPopup();
        }

        ImGui::End();
    }
}  // namespace Ethereal