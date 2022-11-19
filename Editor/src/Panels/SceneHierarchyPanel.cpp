#include "SceneHierarchyPanel.h"

#include "Core/Asset/AssetManager.h"
#include "Base/ImGui/UI.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

namespace Ethereal
{
    extern const std::filesystem::path g_AssetPath;

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene) { SetContext(scene); }

    void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene) {
        m_Context = scene;
        m_SelectionContext = {};
    }

    void SceneHierarchyPanel::OnImGuiRender() {
        ImGui::Begin("Scene Hierarchy");
        if (m_Context) {
            m_Context->m_Registry.each([&](auto entityID) {
                Entity entity{entityID, m_Context.Raw()};
                DrawEntityNode(entity);
            });

            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) m_SelectionContext = {};

            // Right click context menu
            if (ImGui::BeginPopupContextWindow(0, 1, false)) {
                if (ImGui::MenuItem("Create Empty Entity")) m_Context->CreateEntity("Empty Entity");
                if (ImGui::BeginMenu("3D Object")) {
                    if (ImGui::MenuItem("Cube"))
                        m_SelectionContext = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CUBE);
                    if (ImGui::MenuItem("Sphere"))
                        m_SelectionContext = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_SPHERE);
                    if (ImGui::MenuItem("Cylinder"))
                        m_SelectionContext = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CYLINDER);
                    if (ImGui::MenuItem("Cone"))
                        m_SelectionContext = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CONE);
                    if (ImGui::MenuItem("Torus"))
                        m_SelectionContext = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_TORUS);
                    if (ImGui::MenuItem("Plane"))
                        m_SelectionContext = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_PLANE);
                    if (ImGui::MenuItem("Quad"))
                        m_SelectionContext = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_QUAD);
                    if (ImGui::MenuItem("Monkey"))
                        m_SelectionContext = m_Context->Create3DObject(ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_MONKEY);
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Material")) {
                    AssetManager::CreateNewAsset<MaterialAsset>("M_Default.hmaterial", (Project::GetAssetDirectory() / "materials").string(),
                                                                "M_Default");
                }

                ImGui::EndPopup();
            }
        }

        ImGui::End();

        ImGui::Begin("Properties");
        if (m_SelectionContext) {
            DrawComponents(m_SelectionContext);
        }

        ImGui::End();
    }

    void SceneHierarchyPanel::SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }

    static std::tuple<Vector3, Quaternion, Vector3> GetTransformDecomposition(const Matrix4& transform) {
        Vector3 translation, scale, skew;
        Vector4 perspective;
        Quaternion rotation;
        Math::DecomposeTransformMatrix(transform, translation, rotation, scale, skew, perspective);

        return {translation, rotation, scale};
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity) {
        auto& tag = entity.GetComponent<TagComponent>().Tag;

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", tag.c_str());
        if (ImGui::IsItemClicked()) {
            m_SelectionContext = entity;
        }

        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete Entity")) entityDeleted = true;

            ImGui::EndPopup();
        }

        if (opened) {
            if (entity.HasComponent<StaticMeshComponent>()) {
                auto handle = entity.GetComponent<StaticMeshComponent>().StaticMeshHandle;
                auto mesh = AssetManager::GetAsset<StaticMesh>(handle);
                for (auto& submesh : mesh->GetMeshSource()->GetSubmeshes()) {
                    Matrix4 localTransform = submesh.LocalTransform;
                    Matrix4 transform = entity.GetComponent<TransformComponent>().getMatrix() * submesh.Transform;
                    if (ImGui::TreeNode(submesh.NodeName.c_str())) {
                        {
                            auto [translation, rotation, scale] = GetTransformDecomposition(transform);
                            ImGui::Text("World Transform");
                            ImGui::Text("  Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
                            ImGui::Text("  Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
                        }
                        {
                            auto [translation, rotation, scale] = GetTransformDecomposition(localTransform);
                            ImGui::Text("Local Transform");
                            ImGui::Text("  Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
                            ImGui::Text("  Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
                        }
                        ImGui::TreePop();
                    }
                }
            }
            ImGui::TreePop();
        }

        if (entityDeleted) {
            m_Context->DestroyEntity(entity);
            if (m_SelectionContext == entity) m_SelectionContext = {};
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
        if (!m_SelectionContext.HasComponent<T>()) {
            if (ImGui::MenuItem(entryName.c_str())) {
                m_SelectionContext.AddComponent<T>();
                ImGui::CloseCurrentPopup();
            }
        }
    }

}  // namespace Ethereal