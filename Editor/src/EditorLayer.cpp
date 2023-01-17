#include <Resource/ResourceImporter.h>
#include "Core/Asset/AssetManager.h"
#include "EditorLayer.h"
#include "pch.h"

#include "Base/GlobalContext.h"
#include "Core/Scene/Components.h"
#include "Core/Scene/SceneSerializer.h"
#include "Utils/PlatformUtils.h"
#include "imgui.h"
#include "Base/ImGui/ImGuizmo.h"
#include "Base/ImGui/UI.h"

#include "Base/Meta/Serializer.h"
#include "Base/Meta/_generated/serializer/all_serializer.h"
#include "Core/Editor/EditorResource.h"

namespace Ethereal
{
    extern const std::filesystem::path g_AssetPath;

#define SCENE_HIERARCHY_PANEL_ID "SceneHierarchyPanel"
#define CONTENT_BROWSER_PANEL_ID "ContentBrowserPanel"
#define MATERIALS_PANEL_ID "MaterialEditPanel"

    EditorLayer::EditorLayer() : Layer("EditorLayer") {}

    void EditorLayer::OnAttach() {
        EditorResource::Init();
        m_PanelManager = CreateScope<PanelManager>();

        m_PanelManager->AddPanel<SceneHierarchyPanel>(PanelCategory::View, SCENE_HIERARCHY_PANEL_ID, "Scene Hierarchy", true, m_EditorScene, SelectionContext::Scene);
        m_PanelManager->AddPanel<ContentBrowserPanel>(PanelCategory::View, CONTENT_BROWSER_PANEL_ID, "Content Browser", true);
        m_PanelManager->AddPanel<MaterialEditPanel>(PanelCategory::View, MATERIALS_PANEL_ID, "Material", true);


        m_IconPlay = EditorResource::PlayIcon;
        m_IconStop = EditorResource::StopIcon;

        m_EditorCamera = EditorCamera(30.0f, 1280.0f / 720.0f, 0.1f, 500.0f);

        //TODO: Remove this when support default project
        OpenScene("assets/scenes/meta.hscene");
    }

    void EditorLayer::OnDetach() {
        EditorResource::Shutdown();
    }

    void EditorLayer::OnUpdate(TimeStamp ts) {
        switch (m_SceneState) {
            case SceneState::Play: {
                m_RuntimeScene->OnUpdateRuntime(ts);
                break;
            }
            case SceneState::Edit: {
                m_EditorCamera.OnUpdate(ts);
                m_RenderSceneData.ViewProjectionMatrix = m_EditorCamera.GetViewProjection();
                m_RenderSceneData.ViewMatrix = m_EditorCamera.GetViewMatrix();
                m_RenderSceneData.ProjectionMatrix = m_EditorCamera.GetProjection();
                m_RenderSceneData.CameraPosition = m_EditorCamera.GetPosition();
                m_RenderSceneData.NearPlane = m_EditorCamera.GetNearPlane();
                m_RenderSceneData.FarPlane = m_EditorCamera.GetFarPlane();
                m_RenderSceneData.AspectRatio = m_EditorCamera.GetAspectRatio();
                m_RenderSceneData.FOV = m_EditorCamera.GetFOV();
                m_EditorScene->OnUpdateEditor(ts, m_RenderSceneData);

//                std::cout << "View " << Serializer::write(m_RenderSceneData.ViewMatrix.toMatrix4_()) << std::endl;
//                std::cout << "Proj " << Serializer::write(m_RenderSceneData.ProjectionMatrix.toMatrix4_()) << std::endl;
                break;
            }
        }
        GlobalContext::GetRenderSystem().Draw(ts);

        if (m_SceneState == SceneState::Edit) {
            auto [mx, my] = ImGui::GetMousePos();
            mx -= m_ViewportBounds[0].x;
            my -= m_ViewportBounds[0].y;
            Vector2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
            my = viewportSize.y - my;
            int mouseX = (int)mx;
            int mouseY = (int)my;

            if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y) {
                int pixelData = GlobalContext::GetRenderSystem().GetMousePicking(mouseX, mouseY);
                m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_CurrentScene.Raw());
            }
        }
    }

    void EditorLayer::OnImGuiRender() {
        // Note: Switch this to true to enable dockspace
        static bool dockspaceOpen = true;
        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen) {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin()
        // to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen) ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 110.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        style.WindowMinSize.x = minWinSizeX;
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                // ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1
                if (ImGui::MenuItem("New", "Ctrl+N")) NewScene();
                if (ImGui::MenuItem("Open...", "Ctrl+O")) OpenScene();
                if (ImGui::MenuItem("Save", "Ctrl+S")) SaveScene();
                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) SaveSceneAs();
                if (ImGui::MenuItem("Import File", nullptr)) {
                    std::string filepath = FileDialogs::OpenFile("All (*.*)\0*.*\0");
                    if (!filepath.empty()) {
                        ResourceImporter::Import(filepath);
                    }
                }
                if (ImGui::MenuItem("Exit")) Application::Get().Close();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Settings")) {
                ImGui::MenuItem("Skybox", nullptr, &b_ShowSkyboxSettings);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help")) {
                ImGui::MenuItem("Show Demo ImGui", nullptr, &b_ShowDemoImGui);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        if (b_ShowDemoImGui) ImGui::ShowDemoWindow(&b_ShowDemoImGui);
        if (b_ShowSkyboxSettings) ShowSkyboxSettingWindow(&b_ShowSkyboxSettings);
        if (b_ShowProjectSettings) ShowProjectSettingWindow(&b_ShowProjectSettings);

        m_PanelManager->OnImGuiRender();

        ImGui::Begin("Stats");
        std::string name = "None";
        // if (m_HoveredEntity && m_HoveredEntity.HasComponent<TagComponent>()) name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
        ImGui::Text("Hovered Entity: %s", name.c_str());

        const char* drawModeStrings[] = {"FILLED", "LINE", "POINT"};
        const char* currentDrawModeString = drawModeStrings[(int)RenderCommand::GetDrawMode()];
        if (ImGui::BeginCombo("Draw Mode", currentDrawModeString)) {
            for (int i = 0; i < 3; i++) {
                bool isSelected = currentDrawModeString == drawModeStrings[i];
                if (ImGui::Selectable(drawModeStrings[i], isSelected)) {
                    currentDrawModeString = drawModeStrings[i];
                    RenderCommand::SetDrawMode((ETHEREAL_DRAW_MODE)i);
                }

                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::DragFloat3("Directional Light Dir", Math::Ptr(m_RenderSceneData.DirectionalLightDir), 0.1);
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");
        {
            auto viewportOffset = ImGui::GetCursorPos();  // Includes tab bar

            m_ViewportFocused = ImGui::IsWindowFocused();
            m_ViewportHovered = ImGui::IsWindowHovered();
            Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            GlobalContext::SetViewportSize({viewportPanelSize.x, viewportPanelSize.y});
            // Resize
            if (GlobalContext::GetViewportSize().x > 0.0f && GlobalContext::GetViewportSize().y > 0.0f &&  // zero sized framebuffer is invalid
                (GlobalContext::GetRenderSystem().GetMainImageWidth() != GlobalContext::GetViewportSize().x ||
                 GlobalContext::GetRenderSystem().GetMainImageHeight() != GlobalContext::GetViewportSize().y)) {
                GlobalContext::GetRenderSystem().OnResize();
                // m_CameraController.OnResize(GlobalContext::GetViewportSize().x, GlobalContext::GetViewportSize().y);
                m_EditorCamera.SetViewportSize();
                m_CurrentScene->OnViewportResize((uint32_t)GlobalContext::GetViewportSize().x, (uint32_t)GlobalContext::GetViewportSize().y);
            }
            uint64_t textureID = GlobalContext::GetRenderSystem().GetMainImage();
            // ET_CORE_INFO("texture ID {}", textureID);
            ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{GlobalContext::GetViewportSize().x, GlobalContext::GetViewportSize().y},
                         ImVec2{0, 1}, ImVec2{1, 0});

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                    AssetHandle assetHandle = *((AssetHandle*)payload->Data);
                    const AssetMetaData& assetData = AssetManager::GetMetadata(assetHandle);

                    std::filesystem::path workingDirPath = Project::GetAssetDirectory() / assetData.FilePath;

                    if (assetData.Type == AssetType::Scene)
                        OpenScene(workingDirPath.string());
                    else if (assetData.Type == AssetType::StaticMesh)
                        m_EditorScene->CreateEntityWithStaticMesh(assetHandle);
                    else if (assetData.Type == AssetType::Mesh)
                        m_EditorScene->CreateEntityWithMesh(assetHandle);
                }
                ImGui::EndDragDropTarget();
            }

            ImVec2 minBound = ImGui::GetWindowPos();
            minBound.x += viewportOffset.x;
            minBound.y += viewportOffset.y;

            ImVec2 maxBound = {minBound.x + GlobalContext::GetViewportSize().x, minBound.y + GlobalContext::GetViewportSize().y};
            m_ViewportBounds[0] = {minBound.x, minBound.y};
            m_ViewportBounds[1] = {maxBound.x, maxBound.y};

            // Gizmos
            Entity selectedEntity = Entity();
            if(SelectionManager::GetSelections(SelectionContext::Scene).size() > 0) selectedEntity = m_CurrentScene->GetEntityWithUUID(SelectionManager::GetSelections(SelectionContext::Scene)[0]);
            if (selectedEntity && m_GizmoType != -1) {
                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist();

                float windowWidth = (float)ImGui::GetWindowWidth();
                float windowHeight = (float)ImGui::GetWindowHeight();
                ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

                // Editor camera
                const Matrix4& cameraProjection = m_EditorCamera.GetProjection();
                Matrix4 cameraView = m_EditorCamera.GetViewMatrix();

                // Entity transform
                auto& tc = selectedEntity.GetComponent<TransformComponent>();
                Matrix4 transform = tc.getMatrix();

                // Snapping
                bool snap = Input::IsKeyPressed(Key::LeftControl);
                float snapValue = 0.5f;  // Snap to 0.5m for translation/scale
                // Snap to 45 degrees for rotation
                if (m_GizmoType == ImGuizmo::OPERATION::ROTATE) snapValue = 45.0f;

                float snapValues[3] = {snapValue, snapValue, snapValue};

                ImGuizmo::Manipulate(Math::Ptr(cameraView), Math::Ptr(cameraProjection), (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, Math::Ptr(transform),
                                     nullptr, snap ? snapValues : nullptr);
                if (ImGuizmo::IsUsing()) {
                    Vector3 translation, scale, skew;
                    Vector4 perspective;
                    Quaternion rotation;

                    Math::DecomposeTransformMatrix(transform, translation, rotation, scale, skew, perspective);

                    tc.Position = translation;
                    tc.Rotation = rotation;
                    tc.Scale = scale;
                }
            }
        }

        ImGui::End();
        ImGui::PopStyleVar();

        UI_Toolbar();

        ImGui::End();
    }

    void EditorLayer::OnEvent(Event& e) {
        if (m_SceneState == SceneState::Edit) {
            m_EditorCamera.OnEvent(e);
        }
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(ET_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(ET_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
        m_PanelManager->OnEvent(e);
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e) {
        // Shortcuts
        if (e.IsRepeat()) return false;

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
        switch (e.GetKeyCode()) {
            case Key::N: {
                if (control) NewScene();
                break;
            }
            case Key::O: {
                if (control) OpenScene();
                break;
            }
            case Key::S: {
                if (control) {
                    if (shift)
                        SaveSceneAs();
                    else
                        SaveScene();
                }
                break;
            }
            case Key::D: {
                if (control) DuplicateEntity();
                break;
            }
            // Gizmos
            case Key::Q:
                m_GizmoType = -1;
                break;
            case Key::W:
                m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                break;
            case Key::E:
                m_GizmoType = ImGuizmo::OPERATION::ROTATE;
                break;
            case Key::R:
                m_GizmoType = ImGuizmo::OPERATION::SCALE;
                break;
        }
        return true;
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
        if (e.GetMouseButton() == Mouse::ButtonLeft) {
            if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt)) {
                SelectionManager::DeselectAll();
                SelectionManager::Select(SelectionContext::Scene, m_HoveredEntity.GetUUID());
            }
        }
        return false;
    }

    void EditorLayer::NewScene() {
        m_EditorScene = Ref<Scene>::Create();
        m_EditorScene->OnViewportResize((uint32_t)GlobalContext::GetViewportSize().x, (uint32_t)GlobalContext::GetViewportSize().y);
        m_PanelManager->SetSceneContext(m_EditorScene);

        m_EditorScenePath = std::filesystem::path();

        m_CurrentScene = m_EditorScene;
    }

    void EditorLayer::OpenScene() {
        std::string filepath = FileDialogs::OpenFile("Scene (*.hscene)\0*.hscene\0");
        if (!filepath.empty()) {
            OpenScene(std::filesystem::path(filepath));
        }
    }

    void EditorLayer::OpenScene(const std::filesystem::path& path) {
        if (m_SceneState != SceneState::Edit) OnSceneStop();

//        // TODO!: Make sure no panel is drawing entity in current scene, otherwise it will crash
//        m_HoveredEntity = Entity();
//        m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
//        m_MaterialEditPanel.SetSelectEntity(m_HoveredEntity);

        if (path.extension().string() != ".hscene") {
            ET_WARN("Could not load {0} - not a scene file", path.filename().string());
            return;
        }
        m_EditorScene = Ref<Scene>::Create();
        m_EditorScene->OnViewportResize((uint32_t)GlobalContext::GetViewportSize().x, (uint32_t)GlobalContext::GetViewportSize().y);

        m_PanelManager->SetSceneContext(m_EditorScene);

        SceneSerializer serializer(m_EditorScene);
        ET_CORE_ASSERT(serializer.Deserialize(path.string()), "Deserialize Scene {0} failed", path.string());

        m_CurrentScene = m_EditorScene;
        m_EditorScenePath = path;
    }

    void EditorLayer::SaveSceneAs() {
        std::string filepath = FileDialogs::SaveFile("Scene (*.hscene)\0*.hscene\0");
        if (!filepath.empty()) {
            SerializeScene(m_EditorScene, filepath);
            m_EditorScenePath = filepath;
        }
    }

    void EditorLayer::SaveScene() {
        if (!m_EditorScenePath.empty())
            SerializeScene(m_EditorScene, m_EditorScenePath);
        else
            SaveSceneAs();
    }

    void EditorLayer::DuplicateEntity() {
        if (m_SceneState != SceneState::Edit) return;

        Entity selectedEntity;
        if(SelectionManager::GetSelections(SelectionContext::Scene).size() > 0){
            selectedEntity = m_EditorScene->GetEntityWithUUID(SelectionManager::GetSelections(SelectionContext::Scene)[0]);
        }
        if (selectedEntity) m_EditorScene->DuplicateEntity(selectedEntity);
    }

    void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path) {
        SceneSerializer serializer(scene);
        serializer.Serialize(path.string());
    }

    void EditorLayer::OnScenePlay() {
        SelectionManager::DeselectAll();

        // TODO: Save Scene when changed
        m_SceneState = SceneState::Play;
        m_RuntimeScene = Scene::Copy(m_EditorScene);
        m_RuntimeScene->OnRuntimeStart();

        m_PanelManager->SetSceneContext(m_RuntimeScene);
        m_CurrentScene = m_RuntimeScene;
    }

    void EditorLayer::OnSceneStop() {
        m_SceneState = SceneState::Edit;
        m_RuntimeScene->OnRuntimeStop();
        m_RuntimeScene = nullptr;


        m_PanelManager->SetSceneContext(m_EditorScene);
        m_CurrentScene = m_EditorScene;
    }

    void EditorLayer::UI_Toolbar() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        auto& color = ImGui::GetStyle().Colors;
        const auto& buttonHovered = color[ImGuiCol_ButtonHovered];
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        const auto& buttonActive = color[ImGuiCol_ButtonActive];
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

        ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        float size = ImGui::GetWindowHeight() - 4.0f;
        Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

        if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0)) {
            if (m_SceneState == SceneState::Edit) {
                OnScenePlay();
            } else {
                OnSceneStop();
            }
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);
        ImGui::End();
    }

    void EditorLayer::ShowSkyboxSettingWindow(bool* p_open) {
        ImGui::SetNextWindowSize(ImVec2(300, 440), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Skybox", p_open,
                         ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
            ImGui::Separator();
            ImGui::Image(reinterpret_cast<void*>(GlobalContext::GetRenderSystem().GetSkyboxImage()), ImVec2(256, 128), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();
            if (ImGui::Button("Load Skybox")) {
                std::string filepath = FileDialogs::OpenFile("Skybox (*.hdr)\0*.hdr\0");
                if (!filepath.empty()) {
                    if (m_EditorScene) {
                        ET_CORE_INFO("Open Skybox {0}", filepath);
                        m_EditorScene->SetEnvironment(AssetManager::GetAsset<Environment>(filepath));
                    }
                }
            }
            ImGui::Separator();
        }

        ImGui::End();
    }
    void EditorLayer::ShowProjectSettingWindow(bool* p_open) {
        if (ImGui::Begin("Project Settings", p_open, ImGuiWindowFlags_NoCollapse)) {
            if (UI::ComponentHeader("Bloom")) {
                UI::BeginPropertyGrid();
                ImGui::Text("Threshold");
                ImGui::NextColumn();
                ImGui::DragFloat("##BloomThreshold", &Project().GetSettings().bloomSetting.threshold, 0.05f, 0.0f);
                ImGui::NextColumn();

                ImGui::Text("Knee");
                ImGui::NextColumn();
                ImGui::DragFloat("##BloomKnee", &Project().GetSettings().bloomSetting.knee, 0.05f, 0.0f);
                ImGui::NextColumn();

                ImGui::Text("Intensity");
                ImGui::NextColumn();
                ImGui::DragFloat("##BloomIntensity", &Project().GetSettings().bloomSetting.intensity, 0.01f, 0.0f);

                UI::EndPropertyGrid();
            }
        }

        ImGui::End();
    }
}  // namespace Ethereal