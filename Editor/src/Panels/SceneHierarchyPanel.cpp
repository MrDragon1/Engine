#include "SceneHierarchyPanel.h"

#include "Core/Asset/AssetManager.h"
#include "Base/ImGui/UI.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>
#include <Base/ImGui/TreeNode.h>

namespace Ethereal
{
    SelectionContext SceneHierarchyPanel::s_ActiveSelectionContext = SelectionContext::Scene;

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene) { SetContext(scene); }

    void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene) {
        m_Context = scene;
        m_SelectionContext = {};
    }

    void SceneHierarchyPanel::OnImGuiRender() {
        {
            UI::ScopedStyle padding(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("Scene Hierarchy");
        }

        s_ActiveSelectionContext = m_SelectionContext;

        m_IsWindowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
        ImRect windowRect = { ImGui::GetWindowContentRegionMin(), ImGui::GetWindowContentRegionMax() };

        {
            const float edgeOffset = 4.0f;
            UI::ShiftCursorX(edgeOffset * 3.0f);
            UI::ShiftCursorY(edgeOffset * 2.0f);

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - edgeOffset * 3.0f);

            ImGui::Spacing();
            ImGui::Spacing();

            // Entity list
            //------------

            UI::ScopedStyle cellPadding(ImGuiStyleVar_CellPadding, ImVec2(4.0f, 0.0f));

            // Alt row Color
            const ImU32 colRowAlt = UI::ColorWithMultipliedValue(Colors::Theme::backgroundDark, 1.3f);
            UI::ScopedColor tableBGAlt(ImGuiCol_TableRowBgAlt, colRowAlt);

            // Table
            {
                // Scrollable Table uses child window internally
                UI::ScopedColor tableBg(ImGuiCol_ChildBg, Colors::Theme::backgroundDark);

                ImGuiTableFlags tableFlags = ImGuiTableFlags_NoPadInnerX
                                             | ImGuiTableFlags_Resizable
                                             | ImGuiTableFlags_Reorderable
                                             | ImGuiTableFlags_ScrollY
                    /*| ImGuiTableFlags_RowBg *//*| ImGuiTableFlags_Sortable*/;

                const int numColumns = 3;
                if (ImGui::BeginTable("##SceneHierarchy-Table", numColumns, tableFlags, ImVec2(ImGui::GetContentRegionAvail())))
                {

                    ImGui::TableSetupColumn("Label");
                    ImGui::TableSetupColumn("Type");
                    ImGui::TableSetupColumn("Visibility");

                    // Headers
                    {
                        const ImU32 colActive = UI::ColorWithMultipliedValue(Colors::Theme::groupHeader, 1.2f);
                        UI::ScopedColorStack headerColors(ImGuiCol_HeaderHovered, colActive,
                                                            ImGuiCol_HeaderActive, colActive);

                        ImGui::TableSetupScrollFreeze(ImGui::TableGetColumnCount(), 1);

                        ImGui::TableNextRow(ImGuiTableRowFlags_Headers, 22.0f);
                        for (int column = 0; column < ImGui::TableGetColumnCount(); column++)
                        {
                            ImGui::TableSetColumnIndex(column);
                            const char* column_name = ImGui::TableGetColumnName(column);
                            UI::ScopedID columnID(column);

                            UI::ShiftCursor(edgeOffset * 3.0f, edgeOffset * 2.0f);
                            ImGui::TableHeader(column_name);
                            UI::ShiftCursor(-edgeOffset * 3.0f, -edgeOffset * 2.0f);
                        }
                        ImGui::SetCursorPosX(ImGui::GetCurrentTable()->OuterRect.Min.x);
                        UI::Draw::Underline(true, 0.0f, 5.0f);
                    }

                    // List
                    {
                        UI::ScopedColorStack entitySelection(ImGuiCol_Header, IM_COL32_DISABLE,
                                                              ImGuiCol_HeaderHovered, IM_COL32_DISABLE,
                                                              ImGuiCol_HeaderActive, IM_COL32_DISABLE);
                        if(m_Context){
                            m_Context->m_Registry.each([&](auto entityID) {
                                Entity entity{entityID, m_Context.Raw()};
                                DrawEntityNode(entity);
                            });
                        }
                    }

                    if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
                    {
                        DrawEntityCreateMenu({});
                        ImGui::EndPopup();
                    }

                    ImGui::EndTable();
                }
            }
        }

        {
            UI::ScopedStyle windowPadding(ImGuiStyleVar_WindowPadding, ImVec2(2.0, 4.0f));
            ImGui::Begin("Properties");
            const std::vector<UUID> entities = SelectionManager::GetSelections(s_ActiveSelectionContext);
            if(!entities.empty()) DrawComponents(m_Context->GetEntityWithUUID(entities[0]));
            ImGui::End();
        }

        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityCreateMenu(Entity parent){
        Entity newEntity;
        if (ImGui::MenuItem("Create Empty Entity")) m_Context->CreateEntity("Empty Entity");
        if (ImGui::BeginMenu("3D Object")) {
            if (ImGui::MenuItem("Cube"))
                newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CUBE);
            if (ImGui::MenuItem("Sphere"))
                newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_SPHERE);
            if (ImGui::MenuItem("Cylinder"))
                newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CYLINDER);
            if (ImGui::MenuItem("Cone"))
                newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CONE);
            if (ImGui::MenuItem("Torus"))
                newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_TORUS);
            if (ImGui::MenuItem("Plane"))
                newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_PLANE);
            if (ImGui::MenuItem("Quad"))
                newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_QUAD);
            if (ImGui::MenuItem("Monkey"))
                newEntity = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_MONKEY);
            ImGui::EndMenu();
        }
        if (newEntity)
        {
//            if (parent)
//                m_Context->ParentEntity(newEntity, parent);

            SelectionManager::DeselectAll();
            SelectionManager::Select(s_ActiveSelectionContext, newEntity.GetUUID());
        }

        if (ImGui::MenuItem("Material")) {
            MaterialDesc desc;
            desc.Name = "M_Default";
            AssetManager::CreateAsset_Ref("M_Default",(Project::GetAssetDirectory() / "materials").string(), desc);
        }

        ImGui::EndPopup();
    }

    void SceneHierarchyPanel::SetSelectedEntity(Entity entity) { SelectionManager::Select(m_SelectionContext,entity.GetUUID()); }

    static std::tuple<Vector3, Quaternion, Vector3> GetTransformDecomposition(const Matrix4& transform) {
        Vector3 translation, scale, skew;
        Vector4 perspective;
        Quaternion rotation;
        Math::DecomposeTransformMatrix(transform, translation, rotation, scale, skew, perspective);

        return {translation, rotation, scale};
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity) {
        const char* name = "Unnamed Entity";
        if (entity.HasComponent<TagComponent>())
            name = entity.GetComponent<TagComponent>().Tag.c_str();

        const float edgeOffset = 4.0f;
        const float rowHeight = 21.0f;

        // ImGui item height tweaks
        auto* window = ImGui::GetCurrentWindow();
        window->DC.CurrLineSize.y = rowHeight;
        //---------------------------------------------
        ImGui::TableNextRow(0, rowHeight);

        // Label column
        //-------------

        ImGui::TableNextColumn();

        window->DC.CurrLineTextBaseOffset = 3.0f;

        const ImVec2 rowAreaMin = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).Min;
        const ImVec2 rowAreaMax = { ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), ImGui::TableGetColumnCount() - 1).Max.x - 20,
                                   rowAreaMin.y + rowHeight };

        const bool isSelected = SelectionManager::IsSelected(s_ActiveSelectionContext, entity.GetUUID());

        ImGuiTreeNodeFlags flags = (isSelected ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        flags |= ImGuiTreeNodeFlags_Leaf;

        const std::string strID = fmt::format("{0}{1}", name, (uint64_t)entity.GetUUID());

        ImGui::PushClipRect(rowAreaMin, rowAreaMax, false);
        bool isRowHovered, held;
        bool isRowClicked = ImGui::ButtonBehavior(ImRect(rowAreaMin, rowAreaMax), ImGui::GetID(strID.c_str()),
                                                  &isRowHovered, &held, ImGuiButtonFlags_AllowItemOverlap | ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        bool wasRowRightClicked = ImGui::IsMouseReleased(ImGuiMouseButton_Right);

        ImGui::SetItemAllowOverlap();

        ImGui::PopClipRect();

        const bool isWindowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);


        // Row coloring
        //--------------

        // Fill with light selection Color if any of the child entities selected
        auto isAnyDescendantSelected = [&](Entity ent, auto isAnyDescendantSelected) -> bool
        {
            if (SelectionManager::IsSelected(s_ActiveSelectionContext, ent.GetUUID()))
                return true;
            // check children here

            return false;
        };


        auto fillRowWithColor = [](const ImColor& Color)
        {
            for (int column = 0; column < ImGui::TableGetColumnCount(); column++)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, Color, column);
        };

        if (isSelected)
        {
            if (isWindowFocused || UI::NavigatedTo())
                fillRowWithColor(Colors::Theme::selection);
            else
            {
                const ImColor col = UI::ColorWithMultipliedValue(Colors::Theme::selection, 0.9f);
                fillRowWithColor(UI::ColorWithMultipliedSaturation(col, 0.7f));
            }
        }
        else if (isRowHovered)
        {
            fillRowWithColor(Colors::Theme::groupHeader);
        }
        else if (isAnyDescendantSelected(entity, isAnyDescendantSelected))
        {
            fillRowWithColor(Colors::Theme::selectionMuted);
        }

        // Text Coloring
        //---------------

        if (isSelected)
            ImGui::PushStyleColor(ImGuiCol_Text, Colors::Theme::backgroundDark);

        const bool missingStaticMesh = entity.HasComponent<StaticMeshComponent>() && (AssetManager::IsAssetHandleValid(entity.GetComponent<StaticMeshComponent>().StaticMeshHandle)
                                                                          && AssetManager::GetAsset<StaticMesh>(entity.GetComponent<StaticMeshComponent>().StaticMeshHandle) && AssetManager::GetAsset<StaticMesh>(entity.GetComponent<StaticMeshComponent>().StaticMeshHandle)->IsFlagSet(AssetFlag::Missing));
        const bool missingDynamicMesh = entity.HasComponent<MeshComponent>() && (AssetManager::IsAssetHandleValid(entity.GetComponent<MeshComponent>().MeshHandle)
                                                                                 && AssetManager::GetAsset<Mesh>(entity.GetComponent<MeshComponent>().MeshHandle) && AssetManager::GetAsset<Mesh>(entity.GetComponent<MeshComponent>().MeshHandle)->IsFlagSet(AssetFlag::Missing));
        const bool missingMesh = missingDynamicMesh && missingStaticMesh;

        if (missingMesh)
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.4f, 0.3f, 1.0f));

        // Tree node
        //----------
        // TODO: clean up this mess
        ImGuiContext& g = *GImGui;
        auto& style = ImGui::GetStyle();
        const ImVec2 label_size = ImGui::CalcTextSize(strID.c_str(), nullptr, false);
        const ImVec2 padding = ((flags & ImGuiTreeNodeFlags_FramePadding)) ? style.FramePadding : ImVec2(style.FramePadding.x, ImMin(window->DC.CurrLineTextBaseOffset, style.FramePadding.y));
        const float text_offset_x = g.FontSize + padding.x * 2;           // Collapser arrow width + Spacing
        const float text_offset_y = ImMax(padding.y, window->DC.CurrLineTextBaseOffset);                    // Latch before ItemSize changes it
        const float text_width = g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f);  // Include collapser
        ImVec2 text_pos(window->DC.CursorPos.x + text_offset_x, window->DC.CursorPos.y + text_offset_y);
        const float arrow_hit_x1 = (text_pos.x - text_offset_x) - style.TouchExtraPadding.x;
        const float arrow_hit_x2 = (text_pos.x - text_offset_x) + (g.FontSize + padding.x * 2.0f) + style.TouchExtraPadding.x;
        const bool is_mouse_x_over_arrow = (g.IO.MousePos.x >= arrow_hit_x1 && g.IO.MousePos.x < arrow_hit_x2);

        bool previousState = ImGui::TreeNodeBehaviorIsOpen(ImGui::GetID(strID.c_str()));

        if (is_mouse_x_over_arrow && isRowClicked)
            ImGui::SetNextItemOpen(!previousState);

        if (!isSelected && isAnyDescendantSelected(entity, isAnyDescendantSelected))
            ImGui::SetNextItemOpen(true);

        const bool opened = ImGui::TreeNodeWithIcon(nullptr, ImGui::GetID(strID.c_str()), flags, name, nullptr);

        int32_t rowIndex = ImGui::TableGetRowIndex();
        if (rowIndex >= m_FirstSelectedRow && rowIndex <= m_LastSelectedRow && !SelectionManager::IsSelected(entity.GetUUID()))
        {
            SelectionManager::Select(s_ActiveSelectionContext, entity.GetUUID());
        }

        const std::string rightClickPopupID = fmt::format("{0}-ContextMenu", strID);

        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem(rightClickPopupID.c_str()))
        {
            {
                UI::ScopedColor colText(ImGuiCol_Text, Colors::Theme::text);
                UI::ScopedColorStack entitySelection(ImGuiCol_Header, Colors::Theme::groupHeader,
                                                      ImGuiCol_HeaderHovered, Colors::Theme::groupHeader,
                                                      ImGuiCol_HeaderActive, Colors::Theme::groupHeader);

                if (!isSelected)
                {
                    if (!Input::IsKeyPressed(Key::LeftControl))
                        SelectionManager::DeselectAll();

                    SelectionManager::Select(s_ActiveSelectionContext, entity.GetUUID());
                }

                DrawEntityCreateMenu(entity);

                if (ImGui::MenuItem("Delete"))
                    entityDeleted = true;

                ImGui::Separator();

//                if (ImGui::MenuItem("Reset Transform to Mesh"))
//                    m_Context->ResetTransformsToMesh(entity, false);
//
//                if (ImGui::MenuItem("Reset All Transforms to Mesh"))
//                    m_Context->ResetTransformsToMesh(entity, true);
            }

            ImGui::EndPopup();
        }

        // Type column
        //------------
        if (isRowClicked)
        {
            if (wasRowRightClicked)
            {
                ImGui::OpenPopup(rightClickPopupID.c_str());
            }
            else
            {
                if (!Input::IsKeyPressed(Key::LeftControl) && !Input::IsKeyPressed(Key::LeftShift))
                {
                    SelectionManager::DeselectAll();
                    SelectionManager::Select(s_ActiveSelectionContext, entity.GetUUID());
                    m_FirstSelectedRow = rowIndex;
                    m_LastSelectedRow = -1;
                }
                else if (Input::IsKeyPressed(Key::LeftShift) && SelectionManager::GetSelectionCount(s_ActiveSelectionContext) > 0)
                {
                    if (rowIndex < m_FirstSelectedRow)
                    {
                        m_LastSelectedRow = m_FirstSelectedRow;
                        m_FirstSelectedRow = rowIndex;
                    }
                    else
                    {
                        m_LastSelectedRow = rowIndex;
                    }
                }
                else
                {
                    if (isSelected)
                        SelectionManager::Deselect(s_ActiveSelectionContext, entity.GetUUID());
                    else
                        SelectionManager::Select(s_ActiveSelectionContext, entity.GetUUID());
                }
            }

            ImGui::FocusWindow(ImGui::GetCurrentWindow());
        }

        if (missingMesh)
            ImGui::PopStyleColor();

        if (isSelected)
            ImGui::PopStyleColor();


        // Drag & Drop
        //------------
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            const auto& selectedEntities = SelectionManager::GetSelections(s_ActiveSelectionContext);
            UUID entityID = entity.GetUUID();

            if (!SelectionManager::IsSelected(s_ActiveSelectionContext, entityID))
            {
                const char* name = entity.GetName().c_str();
                ImGui::TextUnformatted(name);
                ImGui::SetDragDropPayload("scene_entity_hierarchy", &entityID, 1 * sizeof(UUID));
            }
            else
            {
                for (const auto& selectedEntity : selectedEntities)
                {
                    Entity e = m_Context->GetEntityWithUUID(selectedEntity);
                    const char* name = e.GetName().c_str();
                    ImGui::TextUnformatted(name);
                }

                ImGui::SetDragDropPayload("scene_entity_hierarchy", selectedEntities.data(), selectedEntities.size() * sizeof(UUID));
            }

            ImGui::EndDragDropSource();
        }

        ImGui::TableNextColumn();

        // Draw children
        //--------------

        if (opened)
        {
//            for (auto child : entity.Children())
//                DrawEntityNode(m_Context->GetEntityWithUUID(child), searchFilter);
            ImGui::TreePop();
        }

        // Defer deletion until end of node UI
        if (entityDeleted)
        {
            // NOTE(Peter): Intentional copy since DestroyEntity would call EditorLayer::OnEntityDeleted which deselects the entity
            auto selectedEntities = SelectionManager::GetSelections(s_ActiveSelectionContext);
            for (auto entityID : selectedEntities)
                m_Context->DestroyEntity(m_Context->GetEntityWithUUID(entityID));
        }
    }

    static void DrawVec3Control(const std::string& label, Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();
        ImGui::BeginTable("table_padding", 3, ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoPadInnerX);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X", buttonSize)) values.x = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::SameLine();
        ImGui::TableSetColumnIndex(1);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize)) values.y = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::SameLine();
        ImGui::TableSetColumnIndex(2);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize)) values.z = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopStyleVar();

        ImGui::EndTable();
        ImGui::EndColumns();
    }

    template <typename T, typename UIFunction>
    static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction) {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
                                                 ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
        if (entity.HasComponent<T>()) {
            auto& component = entity.GetComponent<T>();
            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
            ImGui::PopStyleVar();
            ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
            if (ImGui::Button("+", ImVec2{lineHeight, lineHeight})) {
                ImGui::OpenPopup("ComponentSettings");
            }

            bool removeComponent = false;
            if (ImGui::BeginPopup("ComponentSettings")) {
                if (ImGui::MenuItem("Remove component")) removeComponent = true;

                ImGui::EndPopup();
            }

            if (open) {
                uiFunction(component);
                ImGui::TreePop();
            }

            if (removeComponent) entity.RemoveComponent<T>();
        }
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity) {
        if (entity.HasComponent<TagComponent>()) {
            auto& tag = entity.GetComponent<TagComponent>().Tag;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, sizeof(buffer), tag.c_str());
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
                tag = std::string(buffer);
            }
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent")) {
            DisplayAddComponentEntry<CameraComponent>("Camera");
            DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
            DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
            ImGui::EndPopup();
        }
        ImGui::PopItemWidth();

        DrawComponent<TransformComponent>("Transform", entity, [](auto& component) {
            DrawVec3Control("Translation", component.Position);
            Vector3 rotation = Math::Degrees(Vector3(component.Rotation));
            DrawVec3Control("Rotation", rotation);
            component.Rotation = Quaternion(Math::Radians(rotation));
            DrawVec3Control("Scale", component.Scale, 1.0f);
        });

        DrawComponent<StaticMeshComponent>("StaticMesh", entity, [this](auto& component) {
            Ref<StaticMesh> mesh = AssetManager::GetAsset<StaticMesh>(component.StaticMeshHandle);
            Ref<MaterialTable> componentMaterialTable = component.materialTable;
            Ref<MaterialTable> meshMaterialTable = mesh->GetMaterials();

            if (componentMaterialTable) {
                if (meshMaterialTable) {
                    if (componentMaterialTable->GetMaterialCount() < meshMaterialTable->GetMaterialCount())
                        componentMaterialTable->SetMaterialCount(meshMaterialTable->GetMaterialCount());
                }
            }

            AssetHandle meshHandle = component.StaticMeshHandle;
            std::string buttonText = "Null";

            UI::BeginPropertyGrid();

            ImGui::Text("Static Mesh");
            ImGui::NextColumn();
            if (AssetManager::IsAssetHandleValid(meshHandle)) {
                auto object = AssetManager::GetAsset<StaticMesh>(meshHandle);
                if (object && !object->IsFlagSet(AssetFlag::Missing)) {
                    buttonText = AssetManager::GetMetadata(meshHandle).FilePath.stem().string();
                } else {
                    buttonText = "Missing";
                }
            }
            ImGui::Text(buttonText.c_str());

            UI::EndPropertyGrid();

            DrawMaterialTable(componentMaterialTable);
        });

        DrawComponent<MeshComponent>("Mesh", entity, [this](auto& component) {
            Ref<Mesh> mesh = AssetManager::GetAsset<StaticMesh>(component.MeshHandle);
            Ref<MaterialTable> componentMaterialTable = component.materialTable;
            Ref<MaterialTable> meshMaterialTable = mesh->GetMaterials();

            if (componentMaterialTable) {
                if (meshMaterialTable) {
                    if (componentMaterialTable->GetMaterialCount() < meshMaterialTable->GetMaterialCount())
                        componentMaterialTable->SetMaterialCount(meshMaterialTable->GetMaterialCount());
                }
            }

            AssetHandle meshHandle = component.MeshHandle;
            std::string buttonText = "Null";

            UI::BeginPropertyGrid();

            ImGui::Text("Mesh");
            ImGui::NextColumn();
            if (AssetManager::IsAssetHandleValid(meshHandle)) {
                auto object = AssetManager::GetAsset<StaticMesh>(meshHandle);
                if (object && !object->IsFlagSet(AssetFlag::Missing)) {
                    buttonText = AssetManager::GetMetadata(meshHandle).FilePath.stem().string();
                } else {
                    buttonText = "Missing";
                }
            }
            ImGui::Text(buttonText.c_str());

            UI::EndPropertyGrid();

            DrawMaterialTable(componentMaterialTable);

            Ref<Animator> animator = mesh->GetAnimator();
            // TODO: draw animator component
        });

        DrawComponent<CameraComponent>("Camera", entity, [&](auto& cameraComponent) {
            auto& camera = cameraComponent;

            if (ImGui::Checkbox("Primary", &cameraComponent.Camera.Primary)) {
                if (cameraComponent.Camera.Primary) {
                    m_Context->m_Registry.view<CameraComponent>().each([&](const auto otherEntity, auto& cc) { cc.Camera.Primary = otherEntity == entity; });
                }
            }

            const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
            const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.SceneCamera.GetProjectionType()];
            if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) {
                for (int i = 0; i < 2; i++) {
                    bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                    if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
                        currentProjectionTypeString = projectionTypeStrings[i];
                        camera.SceneCamera.SetProjectionType((SceneCamera::ProjectionType)i);
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            if (camera.SceneCamera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
                float verticalFov = Math::Degrees(camera.SceneCamera.GetPerspectiveVerticalFOV());
                if (ImGui::DragFloat("Vertical FOV", &verticalFov)) camera.SceneCamera.SetPerspectiveVerticalFOV(Math::Radians(verticalFov));

                float orthoNear = camera.SceneCamera.GetPerspectiveNearClip();
                if (ImGui::DragFloat("Near", &orthoNear)) camera.SceneCamera.SetPerspectiveNearClip(orthoNear);

                float orthoFar = camera.SceneCamera.GetPerspectiveFarClip();
                if (ImGui::DragFloat("Far", &orthoFar)) camera.SceneCamera.SetPerspectiveFarClip(orthoFar);
            }

            if (camera.SceneCamera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {
                float orthoSize = camera.SceneCamera.GetOrthographicSize();
                if (ImGui::DragFloat("Size", &orthoSize)) camera.SceneCamera.SetOrthographicSize(orthoSize);

                float orthoNear = camera.SceneCamera.GetOrthographicNearClip();
                if (ImGui::DragFloat("Near", &orthoNear)) camera.SceneCamera.SetOrthographicNearClip(orthoNear);

                float orthoFar = camera.SceneCamera.GetOrthographicFarClip();
                if (ImGui::DragFloat("Far", &orthoFar)) camera.SceneCamera.SetOrthographicFarClip(orthoFar);

                ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.Camera.FixedAspectRatio);
            }
        });

        DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component) {
            const char* bodyTypeStrings[] = {"Static", "Dynamic", "Kinematic"};
            const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];

            if (ImGui::BeginCombo("Body Type", currentBodyTypeString)) {
                for (int i = 0; i < 2; i++) {
                    bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
                    if (ImGui::Selectable(bodyTypeStrings[i], isSelected)) {
                        currentBodyTypeString = bodyTypeStrings[i];
                        component.Type = (Rigidbody2DComponent::BodyType)i;
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
        });

        DrawComponent<BoxCollider2DComponent>("BoxCollider 2D", entity, [](auto& component) {
            ImGui::DragFloat2("Offset", Math::Ptr(component.Offset));
            ImGui::DragFloat2("Size", Math::Ptr(component.Size));
            ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
        });
    }

    void SceneHierarchyPanel::DrawMaterialTable(Ref<MaterialTable> materialTable) {
        if (UI::BeginTreeNode("Materials")) {
            for (size_t index = 0; index < materialTable->GetMaterialCount(); index++) {
                UI::BeginPropertyGrid();

                Ref<MaterialAsset> material = materialTable->GetMaterial(index);
                std::string label = fmt::format("[Material {0}]", index);
                ImGui::PushID(label.c_str());

                UI::ShiftCursor(10.0f, 9.0f);
                ImGui::Text(label.c_str());
                ImGui::NextColumn();
                UI::ShiftCursorY(9.0f);

                ImGui::PushItemWidth(-1);
                ImVec2 originalButtonTextAlign = ImGui::GetStyle().ButtonTextAlign;
                ImGui::GetStyle().ButtonTextAlign = {0.0f, 0.5f};
                float width = ImGui::GetContentRegionAvail().x;
                UI::PushID();
                {
                    float itemHeight = 28.0f;
                    std::string materialname = material->GetName();
                    if (materialname.empty()) materialname = "Empty Name";
                    ImGui::Button(materialname.c_str(), ImVec2{width, itemHeight});
                    ImGui::GetStyle().ButtonTextAlign = originalButtonTextAlign;
                }
                UI::PopID();

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                        AssetHandle assetHandle = *((AssetHandle*)payload->Data);
                        Ref<Asset> asset = AssetManager::GetAsset<Asset>(assetHandle);
                        if (asset && asset->GetAssetType() == AssetType::Material) {
                            materialTable->SetMaterial(index, asset.As<MaterialAsset>());
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                UI::DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

                ImGui::PopID();
                UI::EndPropertyGrid();
            }

            UI::EndTreeNode();
        }
    }

    template <typename T>
    void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
//        if (!m_SelectionContext.HasComponent<T>()) {
//            if (ImGui::MenuItem(entryName.c_str())) {
//                m_SelectionContext.AddComponent<T>();
//                ImGui::CloseCurrentPopup();
//            }
//        }
    }

}  // namespace Ethereal