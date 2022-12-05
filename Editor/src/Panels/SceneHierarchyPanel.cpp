#include "SceneHierarchyPanel.h"

#include "Core/Asset/AssetManager.h"
#include "Base/ImGui/UI.h"
#include "Core/Editor/EditorResource.h"

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

                            UI::ShiftCursor(edgeOffset * 3.0f, edgeOffset * 1.0f);
                            ImGui::TableHeader(column_name);
                            UI::ShiftCursor(-edgeOffset * 3.0f, -edgeOffset * 1.0f);
                        }
                        ImGui::SetCursorPosX(ImGui::GetCurrentTable()->OuterRect.Min.x);
                        UI::Draw::Underline(true, 0.0f, 1.0f);
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

            ImGui::Begin("Properties", NULL, ImGuiWindowFlags_NoScrollbar);
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

    void SceneHierarchyPanel::DrawEntityNode(Entity entity) {
        const char* name = "Unnamed Entity";
        if (entity.HasComponent<TagComponent>())
            name = entity.GetComponent<TagComponent>().Tag.c_str();

        const float edgeOffset = 4.0f;
        const float rowHeight = 22.0f;

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

    enum class VectorAxis
    {
        X = BIT(0),
        Y = BIT(1),
        Z = BIT(2),
        W = BIT(3)
    };

    static bool DrawVec3Control(const std::string& label, Vector3& values, bool& manuallyEdited, float resetValue = 0.0f, float columnWidth = 100.0f, uint32_t renderMultiSelectAxes = 0)
    {
        bool modified = false;

        UI::PushID();
        ImGui::TableSetColumnIndex(0);
        UI::ShiftCursor(17.0f, 7.0f);

        ImGui::Text(label.c_str());
        UI::Draw::Underline(false, 0.0f, 2.0f);

        ImGui::TableSetColumnIndex(1);
        UI::ShiftCursor(7.0f, 0.0f);

        {
            const float spacingX = 8.0f;
            UI::ScopedStyle itemSpacing(ImGuiStyleVar_ItemSpacing, ImVec2{ spacingX, 0.0f });
            UI::ScopedStyle padding(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 2.0f });

            {
                // Begin XYZ area
                UI::ScopedColor wpadding(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
                UI::ScopedStyle frame(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));

                ImGui::BeginChild(ImGui::GetID((label + "fr").c_str()),
                                  ImVec2(ImGui::GetContentRegionAvail().x - spacingX, ImGui::GetFrameHeightWithSpacing() + 8.0f),
                                  ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            }
            const float framePadding = 2.0f;
            const float outlineSpacing = 1.0f;
            const float lineHeight = GImGui->Font->FontSize + framePadding * 2.0f;
            const ImVec2 buttonSize = { lineHeight + 2.0f, lineHeight };
            const float inputItemWidth = (ImGui::GetContentRegionAvail().x - spacingX) / 3.0f - buttonSize.x;

            const ImGuiIO& io = ImGui::GetIO();
            auto boldFont = io.Fonts->Fonts[0];

            auto drawControl = [&](const std::string& label, float& value, const ImVec4& colourN,
                                   const ImVec4& colourH,
                                   const ImVec4& colourP, bool renderMultiSelect)
            {
                {
                    UI::ScopedStyle buttonFrame(ImGuiStyleVar_FramePadding, ImVec2(framePadding, 0.0f));
                    UI::ScopedStyle buttonRounding(ImGuiStyleVar_FrameRounding, 1.0f);
                    UI::ScopedColorStack buttonColors(ImGuiCol_Button, colourN,
                                                        ImGuiCol_ButtonHovered, colourH,
                                                        ImGuiCol_ButtonActive, colourP);

                    UI::ScopedFont buttonFont(boldFont);

                    UI::ShiftCursorY(2.0f);
                    if (ImGui::Button(label.c_str(), buttonSize))
                    {
                        value = resetValue;
                        modified = true;
                    }
                }

                ImGui::SameLine(0.0f, outlineSpacing);
                ImGui::SetNextItemWidth(inputItemWidth);
                UI::ShiftCursorY(-2.0f);
                ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, renderMultiSelect);
                bool wasTempInputActive = ImGui::TempInputIsActive(ImGui::GetID(("##" + label).c_str()));
                modified |= UI::DragFloat(("##" + label).c_str(), &value, 0.1f, 0.0f, 0.0f, "%.2f", 0);

                // NOTE(Peter): Ugly hack to make tabbing behave the same as Enter (e.g marking it as manually modified)
                if (modified && Input::IsKeyPressed(Key::Tab))
                    manuallyEdited = true;

                if (ImGui::TempInputIsActive(ImGui::GetID(("##" + label).c_str())))
                    modified = false;

                ImGui::PopItemFlag();

                if (!UI::IsItemDisabled())
                    UI::DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

                if (wasTempInputActive)
                    manuallyEdited |= ImGui::IsItemDeactivatedAfterEdit();
            };

            drawControl("X", values.x, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f }, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f }, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f }, renderMultiSelectAxes & (uint32_t)VectorAxis::X);

            ImGui::SameLine(0.0f, outlineSpacing);
            drawControl("Y", values.y, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f }, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f }, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f }, renderMultiSelectAxes & (uint32_t)VectorAxis::Y);

            ImGui::SameLine(0.0f, outlineSpacing);
            drawControl("Z", values.z, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f }, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f }, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f }, renderMultiSelectAxes & (uint32_t)VectorAxis::Z);

            ImGui::EndChild();
        }
        UI::PopID();

        return modified || manuallyEdited;
    }

    template<typename TComponent>
    void DrawSimpleAddComponentButton(SceneHierarchyPanel* _this, const std::string& name, Ref<Texture2D> icon = nullptr)
    {
        bool canAddComponent = false;

        for (const auto& entityID : SelectionManager::GetSelections(SceneHierarchyPanel::GetActiveSelectionContext()))
        {
            Entity entity = _this->GetSceneContext()->GetEntityWithUUID(entityID);
            if (!entity.HasComponent<TComponent>())
            {
                canAddComponent = true;
                break;
            }
        }

        if (!canAddComponent)
            return;

        if (icon == nullptr)
            icon = EditorResource::AssetIcon;

        const float rowHeight = 25.0f;
        auto* window = ImGui::GetCurrentWindow();
        window->DC.CurrLineSize.y = rowHeight;
        ImGui::TableNextRow(0, rowHeight);
        ImGui::TableSetColumnIndex(0);

        window->DC.CurrLineTextBaseOffset = 3.0f;

        const ImVec2 rowAreaMin = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).Min;
        const ImVec2 rowAreaMax = { ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), ImGui::TableGetColumnCount() - 1).Max.x - 20,
                                   rowAreaMin.y + rowHeight };

        ImGui::PushClipRect(rowAreaMin, rowAreaMax, false);
        bool isRowHovered, held;
        bool isRowClicked = ImGui::ButtonBehavior(ImRect(rowAreaMin, rowAreaMax), ImGui::GetID(name.c_str()), &isRowHovered, &held, ImGuiButtonFlags_AllowItemOverlap);
        ImGui::SetItemAllowOverlap();
        ImGui::PopClipRect();

        auto fillRowWithColour = [](const ImColor& colour)
        {
            for (int column = 0; column < ImGui::TableGetColumnCount(); column++)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, colour, column);
        };

        if (isRowHovered)
            fillRowWithColour(Colors::Theme::background);

        UI::ShiftCursor(1.5f, 1.5f);
        UI::Image(icon, { rowHeight - 3.0f, rowHeight - 3.0f });
        UI::ShiftCursor(-1.5f, -1.5f);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-1);
        ImGui::TextUnformatted(name.c_str());

        if (isRowClicked)
        {
            for (const auto& entityID : SelectionManager::GetSelections(SceneHierarchyPanel::GetActiveSelectionContext()))
            {
                Entity entity = _this->GetSceneContext()->GetEntityWithUUID(entityID);

                if (!entity.HasComponent<TComponent>())
                    entity.AddComponent<TComponent>();
            }

            ImGui::CloseCurrentPopup();
        }
    }

    template<typename TComponent>
    void DrawMaterialTable(SceneHierarchyPanel* _this, const std::vector<UUID>& entities, Ref<MaterialTable> meshMaterialTable, Ref<MaterialTable> localMaterialTable)
    {
        if (UI::BeginTreeNode("Materials"))
        {
            UI::BeginPropertyGrid();

            if (localMaterialTable->GetMaterialCount() < meshMaterialTable->GetMaterialCount())
                localMaterialTable->SetMaterialCount(meshMaterialTable->GetMaterialCount());

            for (size_t i = 0; i < localMaterialTable->GetMaterialCount(); i++)
            {
                if (i == meshMaterialTable->GetMaterialCount())
                    ImGui::Separator();

                bool hasLocalMaterial = localMaterialTable->HasMaterial(i);
                bool hasMeshMaterial = meshMaterialTable->HasMaterial(i);

                std::string label = fmt::format("[Material {0}]", i);

                // NOTE(Peter): Fix for weird ImGui ID bug...
                std::string id = fmt::format("{0}-{1}", label, i);
                ImGui::PushID(id.c_str());

                UI::PropertyAssetReferenceSettings settings;
                if (hasMeshMaterial && !hasLocalMaterial)
                {
                    Ref<MaterialAsset> meshMaterialAsset = meshMaterialTable->GetMaterial(i);
                    std::string meshMaterialName = meshMaterialAsset->GetName();
                    if (meshMaterialName.empty())
                        meshMaterialName = "Unnamed Material";

                    AssetHandle materialAssetHandle = meshMaterialAsset->Handle;

                    ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, false);

                    UI::PropertyAssetReferenceTarget<MaterialAsset>(label.c_str(), meshMaterialName.c_str(), materialAssetHandle, [_this, &entities, i, localMaterialTable](Ref<MaterialAsset> materialAsset) mutable
                        {
                            Ref<Scene> context = _this->GetSceneContext();

                            for (auto entityID : entities)
                            {
                                Entity entity = context->GetEntityWithUUID(entityID);
                                auto& component = entity.GetComponent<TComponent>();

                                if (materialAsset == nullptr)
                                    component.materialTable->ClearMaterial(i);
                                else
                                    component.materialTable->SetMaterial(i, materialAsset);
                            }
                        }, settings);

                    ImGui::PopItemFlag();
                }
                else
                {
                    // hasMeshMaterial is false, hasLocalMaterial could be true or false
                    AssetHandle materialAssetHandle = 0;
                    if (hasLocalMaterial)
                    {
                        materialAssetHandle = localMaterialTable->GetMaterial(i)->Handle;
                        settings.AdvanceToNextColumn = false;
                        settings.WidthOffset = ImGui::GetStyle().ItemSpacing.x + 28.0f;
                    }

                    ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, false);

                    UI::PropertyAssetReferenceTarget<MaterialAsset>(label.c_str(), nullptr, materialAssetHandle, [_this, &entities, i, localMaterialTable](Ref<MaterialAsset> materialAsset) mutable
                        {
                            Ref<Scene> context = _this->GetSceneContext();

                            for (auto entityID : entities)
                            {
                                Entity entity = context->GetEntityWithUUID(entityID);
                                auto& component = entity.GetComponent<TComponent>();

                                if (materialAsset == nullptr)
                                    component.materialTable->ClearMaterial(i);
                                else
                                    component.materialTable->SetMaterial(i, materialAsset);
                            }
                        }, settings);

                    ImGui::PopItemFlag();
                }

                if (hasLocalMaterial)
                {
                    ImGui::SameLine();
                    float prevItemHeight = ImGui::GetItemRectSize().y;
                    if (ImGui::Button(UI::GenerateLabelID("X"), { prevItemHeight, prevItemHeight }))
                        localMaterialTable->ClearMaterial(i);
                    ImGui::NextColumn();
                }

                ImGui::PopID();
            }

            UI::EndPropertyGrid();
            UI::EndTreeNode();
        }
    }



    template<typename TComponent, typename UIFunction>
    void SceneHierarchyPanel::DrawComponent(const std::string& name, UIFunction uiFunction, Ref<Texture2D> icon)
    {
        bool shouldDraw = true;

        auto& entities = SelectionManager::GetSelections(s_ActiveSelectionContext);
        for (const auto& entityID : entities)
        {
            Entity entity = m_Context->GetEntityWithUUID(entityID);
            if (!entity.HasComponent<TComponent>())
            {
                shouldDraw = false;
                break;
            }
        }

        if (!shouldDraw)
            return;

        if (icon == nullptr)
            icon = EditorResource::AssetIcon;

        // NOTE(Peter):
        //	This fixes an issue where the first "+" button would display the "Remove" buttons for ALL components on an Entity.
        //	This is due to ImGui::TreeNodeEx only pushing the id for it's children if it's actually open
        ImGui::PushID((void*)typeid(TComponent).hash_code());
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

        bool open = UI::TreeNodeWithIcon(name, icon, { 20.0f, 20.0f });
        bool right_clicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
        float lineHeight = ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y;

        bool resetValues = false;
        bool removeComponent = false;

        ImGui::SameLine(contentRegionAvailable.x - lineHeight - 5.0f);
        UI::ShiftCursorY(lineHeight / 4.0f);
        if (ImGui::InvisibleButton("##options", ImVec2{ lineHeight, lineHeight }) || right_clicked)
        {
            ImGui::OpenPopup("ComponentSettings");
        }
        UI::DrawButtonImage(EditorResource::GearIcon, COLOR32(160, 160, 160, 200),
                            COLOR32(160, 160, 160, 255),
                            COLOR32(160, 160, 160, 150),
                            UI::RectExpanded(UI::GetItemRect(), 0.0f, 0.0f));

        if (UI::BeginPopup("ComponentSettings"))
        {
            Entity entity = m_Context->GetEntityWithUUID(entities[0]);
            auto& component = entity.GetComponent<TComponent>();

            if (ImGui::MenuItem("Copy"))
                //Scene::CopyComponentFromScene<TComponent>(m_ComponentCopyEntity, m_ComponentCopyScene, entity, m_Context);

            if (ImGui::MenuItem("Paste"))
            {
//                for (auto entityID : SelectionManager::GetSelections(s_ActiveSelectionContext))
//                {
//                    Entity selectedEntity = m_Context->GetEntityWithUUID(entityID);
//                    Scene::CopyComponentFromScene<TComponent>(selectedEntity, m_Context, m_ComponentCopyEntity, m_ComponentCopyScene);
//                }
            }

            if (ImGui::MenuItem("Reset"))
                resetValues = true;

            if constexpr (!std::is_same<TComponent, TransformComponent>::value)
            {
                if (ImGui::MenuItem("Remove component"))
                    removeComponent = true;
            }

            UI::EndPopup();
        }

        if (open)
        {
            Entity entity = m_Context->GetEntityWithUUID(entities[0]);
            TComponent& firstComponent = entity.GetComponent<TComponent>();
            const bool isMultiEdit = entities.size() > 1;
            uiFunction(firstComponent, entities, isMultiEdit);
            ImGui::TreePop();
        }

        if (removeComponent)
        {
            for (auto& entityID : entities)
            {
                Entity entity = m_Context->GetEntityWithUUID(entityID);
                if (entity.HasComponent<TComponent>())
                    entity.RemoveComponent<TComponent>();
            }
        }

        if (resetValues)
        {
            for (auto& entityID : entities)
            {
                Entity entity = m_Context->GetEntityWithUUID(entityID);
                if (entity.HasComponent<TComponent>())
                {
                    entity.RemoveComponent<TComponent>();
                    entity.AddComponent<TComponent>();
                }
            }
        }

        if (!open)
            UI::ShiftCursorY(-(ImGui::GetStyle().ItemSpacing.y + 1.0f));

        ImGui::PopID();
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity) {
        ImGui::AlignTextToFramePadding();

        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

        UI::ShiftCursor(4.0f, 4.0f);

        bool isHoveringID = false;
        // Draw Tag Field
        {
            const float iconOffset = 6.0f;
            UI::ShiftCursor(4.0f, iconOffset);
            UI::Image(EditorResource::PencilIcon, ImVec2(EditorResource::PencilIcon->GetWidth(), EditorResource::PencilIcon->GetHeight()),
                      ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f),
                      ImColor(128, 128, 128, 255).Value);

            ImGui::SameLine(0.0f, 4.0f);
            UI::ShiftCursorY(-iconOffset);

            const std::string& tag = entity.GetName();

            char buffer[256];
            memset(buffer, 0, 256);
            memcpy(buffer, tag.c_str(), tag.length());
            ImGui::PushItemWidth(contentRegionAvailable.x * 0.5f);
            UI::ScopedStyle frameBorder(ImGuiStyleVar_FrameBorderSize, 0.0f);
            UI::ScopedColor frameColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
            UI::ScopedFont boldFont(ImGui::GetIO().Fonts->Fonts[0]);

            if (ImGui::InputText("##Tag", buffer, 256))
            {
                entity.GetComponent<TagComponent>().Tag = std::string(buffer);
            }

            UI::DrawItemActivityOutline(2.0f, false, Colors::Theme::accent);

            isHoveringID = ImGui::IsItemHovered();

            ImGui::PopItemWidth();
        }

        ImGui::SameLine();
        UI::ShiftCursorX(-5.0f);

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

        ImVec2 addTextSize = ImGui::CalcTextSize(" ADD        ");
        addTextSize.x += GImGui->Style.FramePadding.x * 2.0f;
        {
            UI::ScopedColorStack addCompButtonColors(ImGuiCol_Button, IM_COL32(70, 70, 70, 200),
                                                       ImGuiCol_ButtonHovered, IM_COL32(70, 70, 70, 255),
                                                       ImGuiCol_ButtonActive, IM_COL32(70, 70, 70, 150));

            ImGui::SameLine(contentRegionAvailable.x - (addTextSize.x + GImGui->Style.FramePadding.x));
            if (ImGui::Button(" ADD       ", ImVec2(addTextSize.x + 4.0f, lineHeight + 2.0f)))
                ImGui::OpenPopup("AddComponentPanel");

            const float pad = 4.0f;
            const float iconWidth = ImGui::GetFrameHeight() - pad * 2.0f;
            const float iconHeight = iconWidth;
            ImVec2 iconPos = ImGui::GetItemRectMax();
            iconPos.x -= iconWidth + pad;
            iconPos.y -= iconHeight + pad;
            ImGui::SetCursorScreenPos(iconPos);
            UI::ShiftCursor(-5.0f, -1.0f);

            UI::Image(EditorResource::PlusIcon, ImVec2(iconWidth, iconHeight),
                      ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f),
                      ImColor(160, 160, 160, 255).Value);
        }

        float addComponentPanelStartY = ImGui::GetCursorScreenPos().y;

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        {
            UI::ScopedFont boldFont(ImGui::GetIO().Fonts->Fonts[0]);
            UI::ScopedStyle itemSpacing(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            UI::ScopedStyle windowPadding(ImGuiStyleVar_WindowPadding, ImVec2(5, 10));
            UI::ScopedStyle windowRounding(ImGuiStyleVar_PopupRounding, 4.0f);
            UI::ScopedStyle cellPadding(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0));

            static float addComponentPanelWidth = 250.0f;
            const float maxHeight = ImGui::GetContentRegionMax().y - 60.0f;

            ImGui::SetNextWindowPos({ ImGui::GetContentRegionAvail().x - addComponentPanelWidth, addComponentPanelStartY});
            ImGui::SetNextWindowSizeConstraints({ 50.0f, 50.0f }, { addComponentPanelWidth, maxHeight });
            if (ImGui::BeginPopup("AddComponentPanel", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking))
            {
                // Setup Table
                if (ImGui::BeginTable("##component_table", 2, ImGuiTableFlags_SizingStretchSame))
                {
                    ImGui::TableSetupColumn("Icon", ImGuiTableColumnFlags_WidthFixed, addComponentPanelWidth * 0.15f);
                    ImGui::TableSetupColumn("ComponentNames", ImGuiTableColumnFlags_WidthFixed, addComponentPanelWidth * 0.85f);

                    DrawSimpleAddComponentButton<CameraComponent>(this, "Camera", EditorResource::CameraIcon);
                    DrawSimpleAddComponentButton<MeshComponent>(this, "Mesh");
                    DrawSimpleAddComponentButton<StaticMeshComponent>(this, "Static Mesh");

                    ImGui::EndTable();
                }

                ImGui::EndPopup();
            }
        }

        DrawComponent<TransformComponent>("Transform", [&](TransformComponent& firstComponent, const std::vector<UUID>& entities, const bool isMultiEdit)
                                          {
                                              UI::ScopedStyle spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
                                              UI::ScopedStyle padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

                                              ImGui::BeginTable("transformComponent", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoClip);
                                              ImGui::TableSetupColumn("label_column", 0, 100.0f);
                                              ImGui::TableSetupColumn("value_column", ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoClip, ImGui::GetContentRegionAvail().x - 100.0f);

                                              bool translationManuallyEdited = false;
                                              bool rotationManuallyEdited = false;
                                              bool scaleManuallyEdited = false;


                                              // Not support multi edit for now
                                              Entity entity = m_Context->GetEntityWithUUID(entities[0]);
                                              auto& component = entity.GetComponent<TransformComponent>();

                                              ImGui::TableNextRow();
                                              DrawVec3Control("Translation", component.Position, translationManuallyEdited);

                                              ImGui::TableNextRow();
                                              Vector3 rotation = Math::Degrees(component.Rotation);
                                              DrawVec3Control("Rotation", rotation, rotationManuallyEdited);
                                              component.Rotation = Math::Radians(rotation);

                                              ImGui::TableNextRow();
                                              DrawVec3Control("Scale", component.Scale, scaleManuallyEdited, 1.0f);


                                              ImGui::EndTable();

                                              UI::ShiftCursorY(-8.0f);
                                              UI::Draw::Underline();

                                              UI::ShiftCursorY(18.0f);
                                          });

        DrawComponent<MeshComponent>("Mesh", [&](MeshComponent& firstComponent, const std::vector<UUID>& entities, const bool isMultiEdit)
                                     {
                                         AssetHandle meshHandle = firstComponent.MeshHandle;
                                         Ref<Mesh> mesh = AssetManager::GetAsset<Mesh>(meshHandle);

                                         UI::BeginPropertyGrid();
                                         ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, false);

                                         mesh = AssetManager::GetAsset<Mesh>(meshHandle);

                                         for (auto& entityID : entities)
                                         {
                                             Entity entity = m_Context->GetEntityWithUUID(entityID);
                                             auto& mc = entity.GetComponent<MeshComponent>();
                                             mc.MeshHandle = meshHandle;

                                             if (mesh)
                                             {
                                                 // Validate submesh index

                                                 // TODO!: Support submesh index
                                             }
                                         }


                                         ImGui::PopItemFlag();

                                         UI::EndPropertyGrid();

                                         if (mesh && mesh->IsValid())
                                             DrawMaterialTable<MeshComponent>(this, entities, mesh->GetMaterials(), firstComponent.materialTable);
                                     });

        DrawComponent<StaticMeshComponent>("Static Mesh", [&](StaticMeshComponent& firstComponent, const std::vector<UUID>& entities, const bool isMultiEdit)
                                           {
                                               Ref<StaticMesh> mesh = AssetManager::GetAsset<StaticMesh>(firstComponent.StaticMeshHandle);
                                               AssetHandle meshHandle = firstComponent.StaticMeshHandle;

                                               UI::BeginPropertyGrid();
                                               ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, false);

                                               ImGui::PopItemFlag();

                                               UI::EndPropertyGrid();

                                               if (mesh && mesh->IsValid())
                                                   DrawMaterialTable<StaticMeshComponent>(this, entities, mesh->GetMaterials(), firstComponent.materialTable);
                                           });





        // Useless
//        {
//            if (entity.HasComponent<TagComponent>()) {
//                auto& tag = entity.GetComponent<TagComponent>().Tag;
//
//                char buffer[256];
//                memset(buffer, 0, sizeof(buffer));
//                strcpy_s(buffer, sizeof(buffer), tag.c_str());
//                if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
//                    tag = std::string(buffer);
//                }
//            }
//            ImGui::SameLine();
//            ImGui::PushItemWidth(-1);
//
//            if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponent");
//
//            if (ImGui::BeginPopup("AddComponent")) {
//                DisplayAddComponentEntry<CameraComponent>("Camera");
//                DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
//                DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
//                ImGui::EndPopup();
//            }
//            ImGui::PopItemWidth();
//
//            DrawComponent<TransformComponent>("Transform", entity, [](auto& component) {
//                DrawVec3Control("Translation", component.Position);
//                Vector3 rotation = Math::Degrees(Vector3(component.Rotation));
//                DrawVec3Control("Rotation", rotation);
//                component.Rotation = Quaternion(Math::Radians(rotation));
//                DrawVec3Control("Scale", component.Scale, 1.0f);
//            });
//
//            DrawComponent<StaticMeshComponent>("StaticMesh", entity, [this](auto& component) {
//                Ref<StaticMesh> mesh = AssetManager::GetAsset<StaticMesh>(component.StaticMeshHandle);
//                Ref<MaterialTable> componentMaterialTable = component.materialTable;
//                Ref<MaterialTable> meshMaterialTable = mesh->GetMaterials();
//
//                if (componentMaterialTable) {
//                    if (meshMaterialTable) {
//                        if (componentMaterialTable->GetMaterialCount() < meshMaterialTable->GetMaterialCount())
//                            componentMaterialTable->SetMaterialCount(meshMaterialTable->GetMaterialCount());
//                    }
//                }
//
//                AssetHandle meshHandle = component.StaticMeshHandle;
//                std::string buttonText = "Null";
//
//                UI::BeginPropertyGrid();
//
//                ImGui::Text("Static Mesh");
//                ImGui::NextColumn();
//                if (AssetManager::IsAssetHandleValid(meshHandle)) {
//                    auto object = AssetManager::GetAsset<StaticMesh>(meshHandle);
//                    if (object && !object->IsFlagSet(AssetFlag::Missing)) {
//                        buttonText = AssetManager::GetMetadata(meshHandle).FilePath.stem().string();
//                    } else {
//                        buttonText = "Missing";
//                    }
//                }
//                ImGui::Text(buttonText.c_str());
//
//                UI::EndPropertyGrid();
//
//                DrawMaterialTable(componentMaterialTable);
//            });
//
//            DrawComponent<MeshComponent>("Mesh", entity, [this](auto& component) {
//                Ref<Mesh> mesh = AssetManager::GetAsset<StaticMesh>(component.MeshHandle);
//                Ref<MaterialTable> componentMaterialTable = component.materialTable;
//                Ref<MaterialTable> meshMaterialTable = mesh->GetMaterials();
//
//                if (componentMaterialTable) {
//                    if (meshMaterialTable) {
//                        if (componentMaterialTable->GetMaterialCount() < meshMaterialTable->GetMaterialCount())
//                            componentMaterialTable->SetMaterialCount(meshMaterialTable->GetMaterialCount());
//                    }
//                }
//
//                AssetHandle meshHandle = component.MeshHandle;
//                std::string buttonText = "Null";
//
//                UI::BeginPropertyGrid();
//
//                ImGui::Text("Mesh");
//                ImGui::NextColumn();
//                if (AssetManager::IsAssetHandleValid(meshHandle)) {
//                    auto object = AssetManager::GetAsset<StaticMesh>(meshHandle);
//                    if (object && !object->IsFlagSet(AssetFlag::Missing)) {
//                        buttonText = AssetManager::GetMetadata(meshHandle).FilePath.stem().string();
//                    } else {
//                        buttonText = "Missing";
//                    }
//                }
//                ImGui::Text(buttonText.c_str());
//
//                UI::EndPropertyGrid();
//
//                DrawMaterialTable(componentMaterialTable);
//
//                Ref<Animator> animator = mesh->GetAnimator();
//                // TODO: draw animator component
//            });
//
//            DrawComponent<CameraComponent>("Camera", entity, [&](auto& cameraComponent) {
//                auto& camera = cameraComponent;
//
//                if (ImGui::Checkbox("Primary", &cameraComponent.Camera.Primary)) {
//                    if (cameraComponent.Camera.Primary) {
//                        m_Context->m_Registry.view<CameraComponent>().each([&](const auto otherEntity, auto& cc) { cc.Camera.Primary = otherEntity == entity; });
//                    }
//                }
//
//                const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
//                const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.SceneCamera.GetProjectionType()];
//                if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) {
//                    for (int i = 0; i < 2; i++) {
//                        bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
//                        if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
//                            currentProjectionTypeString = projectionTypeStrings[i];
//                            camera.SceneCamera.SetProjectionType((SceneCamera::ProjectionType)i);
//                        }
//
//                        if (isSelected) ImGui::SetItemDefaultFocus();
//                    }
//
//                    ImGui::EndCombo();
//                }
//
//                if (camera.SceneCamera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
//                    float verticalFov = Math::Degrees(camera.SceneCamera.GetPerspectiveVerticalFOV());
//                    if (ImGui::DragFloat("Vertical FOV", &verticalFov)) camera.SceneCamera.SetPerspectiveVerticalFOV(Math::Radians(verticalFov));
//
//                    float orthoNear = camera.SceneCamera.GetPerspectiveNearClip();
//                    if (ImGui::DragFloat("Near", &orthoNear)) camera.SceneCamera.SetPerspectiveNearClip(orthoNear);
//
//                    float orthoFar = camera.SceneCamera.GetPerspectiveFarClip();
//                    if (ImGui::DragFloat("Far", &orthoFar)) camera.SceneCamera.SetPerspectiveFarClip(orthoFar);
//                }
//
//                if (camera.SceneCamera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {
//                    float orthoSize = camera.SceneCamera.GetOrthographicSize();
//                    if (ImGui::DragFloat("Size", &orthoSize)) camera.SceneCamera.SetOrthographicSize(orthoSize);
//
//                    float orthoNear = camera.SceneCamera.GetOrthographicNearClip();
//                    if (ImGui::DragFloat("Near", &orthoNear)) camera.SceneCamera.SetOrthographicNearClip(orthoNear);
//
//                    float orthoFar = camera.SceneCamera.GetOrthographicFarClip();
//                    if (ImGui::DragFloat("Far", &orthoFar)) camera.SceneCamera.SetOrthographicFarClip(orthoFar);
//
//                    ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.Camera.FixedAspectRatio);
//                }
//            });
//
//            DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component) {
//                const char* bodyTypeStrings[] = {"Static", "Dynamic", "Kinematic"};
//                const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
//
//                if (ImGui::BeginCombo("Body Type", currentBodyTypeString)) {
//                    for (int i = 0; i < 2; i++) {
//                        bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
//                        if (ImGui::Selectable(bodyTypeStrings[i], isSelected)) {
//                            currentBodyTypeString = bodyTypeStrings[i];
//                            component.Type = (Rigidbody2DComponent::BodyType)i;
//                        }
//
//                        if (isSelected) ImGui::SetItemDefaultFocus();
//                    }
//                    ImGui::EndCombo();
//                }
//
//                ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
//            });
//
//            DrawComponent<BoxCollider2DComponent>("BoxCollider 2D", entity, [](auto& component) {
//                ImGui::DragFloat2("Offset", Math::Ptr(component.Offset));
//                ImGui::DragFloat2("Size", Math::Ptr(component.Size));
//                ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
//                ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
//                ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
//                ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
//            });
//        }
    }
}  // namespace Ethereal