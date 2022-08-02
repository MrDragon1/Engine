#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

#include "Utils/AssetManager.h"

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
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
            ;
            bool opened = ImGui::TreeNodeEx((void*)9817239, flags, "%s", tag.c_str());
            if (opened) ImGui::TreePop();
            ImGui::TreePop();
        }

        if (entityDeleted) {
            m_Context->DestroyEntity(entity);
            if (m_SelectionContext == entity) m_SelectionContext = {};
        }
    }
    static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
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
            DrawVec3Control("Translation", component.Translation);
            glm::vec3 rotation = glm::degrees(component.Rotation);
            DrawVec3Control("Rotation", rotation);
            component.Rotation = glm::radians(rotation);
            DrawVec3Control("Scale", component.Scale, 1.0f);
        });

        DrawComponent<CameraComponent>("Camera", entity, [&](auto& cameraComponent) {
            auto& camera = cameraComponent.Camera;

            if (ImGui::Checkbox("Primary", &cameraComponent.Primary)) {
                if (cameraComponent.Primary) {
                    m_Context->m_Registry.view<CameraComponent>().each([&](const auto otherEntity, auto& cc) { cc.Primary = otherEntity == entity; });
                }
            }

            const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
            const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
            if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) {
                for (int i = 0; i < 2; i++) {
                    bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                    if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
                        currentProjectionTypeString = projectionTypeStrings[i];
                        camera.SetProjectionType((SceneCamera::ProjectionType)i);
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
                float verticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
                if (ImGui::DragFloat("Vertical FOV", &verticalFov)) camera.SetPerspectiveVerticalFOV(glm::radians(verticalFov));

                float orthoNear = camera.GetPerspectiveNearClip();
                if (ImGui::DragFloat("Near", &orthoNear)) camera.SetPerspectiveNearClip(orthoNear);

                float orthoFar = camera.GetPerspectiveFarClip();
                if (ImGui::DragFloat("Far", &orthoFar)) camera.SetPerspectiveFarClip(orthoFar);
            }

            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {
                float orthoSize = camera.GetOrthographicSize();
                if (ImGui::DragFloat("Size", &orthoSize)) camera.SetOrthographicSize(orthoSize);

                float orthoNear = camera.GetOrthographicNearClip();
                if (ImGui::DragFloat("Near", &orthoNear)) camera.SetOrthographicNearClip(orthoNear);

                float orthoFar = camera.GetOrthographicFarClip();
                if (ImGui::DragFloat("Far", &orthoFar)) camera.SetOrthographicFarClip(orthoFar);

                ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
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
            ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
            ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
            ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
        });

        // TODO: Beautify this
        DrawComponent<MaterialComponent>("Material", entity, [](auto& component) {
            const auto& materialNode = [&component](const char* name, std::string& file, void (*func)(MaterialComponent & components)) {
                ET_CORE_ASSERT(!file.empty(), "Material {0} file is null", name);
                Ref<Texture> showTexture = TextureManager::AddTexture(file);

                if (ImGui::TreeNode((void*)name, name)) {
                    ImGui::Image((ImTextureID)showTexture->GetRendererID(), ImVec2(64, 64), ImVec2{0, 1}, ImVec2{1, 0});
                    static bool use = false;
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                            const wchar_t* path = (const wchar_t*)payload->Data;
                            std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
                            Ref<Texture> texture = TextureManager::AddTexture(texturePath.string());
                            if (texture)
                                file = texturePath.string();
                            else
                                ET_WARN("Could not load texture {0}", texturePath.filename().string());
                        }
                        ImGui::EndDragDropTarget();
                    }

                    func(component);

                    ImGui::TreePop();
                }
            };

            materialNode("Albedo", component.Desc.m_AlbedoFile, [](MaterialComponent& components) {
                static bool use = false;
                ImGui::SameLine();
                if (ImGui::Checkbox("Use", &use)) {
                    components.Desc.b_Albedo = use;
                }
            });
            materialNode("Normal", component.Desc.m_NormalFile, [](MaterialComponent& components) {
                static bool use = true;
                ImGui::SameLine();
                if (ImGui::Checkbox("Use", &use)) {
                    components.Desc.b_Normal = use;
                }
            });
            materialNode("Metallic", component.Desc.m_MetallicFile, [](MaterialComponent& components) {
                static bool use = false;
                ImGui::SameLine();
                if (ImGui::Checkbox("Use", &use)) {
                    components.Desc.b_Metallic = use;
                }
            });
            materialNode("Roughness", component.Desc.m_RoughnessFile, [](MaterialComponent& components) {
                static bool use = false;
                ImGui::SameLine();
                if (ImGui::Checkbox("Use", &use)) {
                    components.Desc.b_Roughness = use;
                }
            });
            materialNode("Occlusion", component.Desc.m_OcclusionFile, [](MaterialComponent& components) {
                static bool use = false;
                ImGui::SameLine();
                if (ImGui::Checkbox("Use", &use)) {
                    components.Desc.b_Occlusion = use;
                }
            });
            materialNode("Emssive", component.Desc.m_EmissiveFile, [](MaterialComponent& components) {
                static bool use = false;
                ImGui::SameLine();
                ImGui::Checkbox("Use", &use);
            });

            ImGui::ColorEdit3("Albedo", glm::value_ptr(component.Desc.m_Albedo));
            ImGui::DragFloat("Metallic", &component.Desc.m_Metallic, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Roughness", &component.Desc.m_Roughness, 0.01f, 0.0f, 1.0f);
        });
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