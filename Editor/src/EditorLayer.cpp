#include <Asset/AssetManager.h>
#include "EditorLayer.h"
#include "pch.h"

#include "Core/GlobalContext.h"
#include "Scene/Components.h"
#include "Scene/SceneSerializer.h"
#include "Utils/Math.h"
#include "Utils/PlatformUtils.h"
#include "imgui.h"
#include "ImGui/ImGuizmo.h"

namespace Ethereal
{
    extern const std::filesystem::path g_AssetPath;

    EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f) {}

    void EditorLayer::OnAttach() {
        m_IconPlay = Texture2D::Create("assets/icons/PlayButton.png");
        m_IconStop = Texture2D::Create("assets/icons/StopButton.png");

        FramebufferSpecification fbSpec;
        fbSpec.Attachments = {ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R8G8B8A8_UNORM, ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R32_INTEGER,
                              ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_DEPTH};
        fbSpec.Width = 1280;
        fbSpec.Height = 720;

        m_ActiveScene = Ref<Scene>::Create();
        m_EditorCamera = EditorCamera(30.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
    }

    void EditorLayer::OnDetach() {}

    void EditorLayer::OnUpdate(Timestep ts) {
        switch (m_SceneState) {
            case SceneState::Play: {
                m_ActiveScene->OnUpdateRuntime(ts);
                break;
            }
            case SceneState::Edit: {
                m_EditorCamera.OnUpdate(ts);
                m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
                break;
            }
        }
        GlobalContext::GetRenderSystem().m_ShadowMapRenderPass->SetLightPosition(m_LightPos);
        GlobalContext::GetRenderSystem().Draw(ts);

        if (m_SceneState == SceneState::Edit) {
            auto [mx, my] = ImGui::GetMousePos();
            mx -= m_ViewportBounds[0].x;
            my -= m_ViewportBounds[0].y;
            glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
            my = viewportSize.y - my;
            int mouseX = (int)mx;
            int mouseY = (int)my;

            if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y) {
                int pixelData = GlobalContext::GetRenderSystem().GetMousePicking(mouseX, mouseY);
                // ET_CORE_INFO("Pixel data: {0}", pixelData);
                m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.Raw());
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
        static bool bShowDemoImGui = false;
        static bool bShowSkyboxSettings = false;
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
                        AssetManager::ImportAsset(std::filesystem::path(filepath));
                    }
                }
                if (ImGui::MenuItem("Exit")) Application::Get().Close();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Settings")) {
                ImGui::MenuItem("Skybox", nullptr, &bShowSkyboxSettings);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help")) {
                ImGui::MenuItem("Show Demo ImGui", nullptr, &bShowDemoImGui);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        if (bShowDemoImGui) ImGui::ShowDemoWindow(&bShowDemoImGui);
        if (bShowSkyboxSettings) ShowSkyboxSettingWindow(&bShowSkyboxSettings);

        m_SceneHierarchyPanel.OnImGuiRender();
        m_ContentBrowserPanel.OnImGuiRender();
        m_MaterialEditPanel.OnImGuiRender();

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

        ImGui::DragFloat3("Light Position", glm::value_ptr(m_LightPos), 0.1);
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");
        {
            auto viewportOffset = ImGui::GetCursorPos();  // Includes tab bar

            m_ViewportFocused = ImGui::IsWindowFocused();
            m_ViewportHovered = ImGui::IsWindowHovered();
            Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            GlobalContext::SetViewportSize({viewportPanelSize.x, viewportPanelSize.y});
            // Resize
            if (GlobalContext::GetViewportSize().x > 0.0f && GlobalContext::GetViewportSize().y > 0.0f &&  // zero sized framebuffer is invalid
                (GlobalContext::GetRenderSystem().GetMainImageWidth() != GlobalContext::GetViewportSize().x ||
                 GlobalContext::GetRenderSystem().GetMainImageHeight() != GlobalContext::GetViewportSize().y)) {
                GlobalContext::GetRenderSystem().OnResize();
                // m_CameraController.OnResize(GlobalContext::GetViewportSize().x, GlobalContext::GetViewportSize().y);
                m_EditorCamera.SetViewportSize();
                m_ActiveScene->OnViewportResize((uint32_t)GlobalContext::GetViewportSize().x, (uint32_t)GlobalContext::GetViewportSize().y);
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
                    OpenScene(workingDirPath.string());
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
            Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
            if (selectedEntity && m_GizmoType != -1) {
                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist();

                float windowWidth = (float)ImGui::GetWindowWidth();
                float windowHeight = (float)ImGui::GetWindowHeight();
                ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

                // Editor camera
                const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
                glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

                // Entity transform
                auto& tc = selectedEntity.GetComponent<TransformComponent>();
                glm::mat4 transform = tc.GetTransform();

                // Snapping
                bool snap = Input::IsKeyPressed(Key::LeftControl);
                float snapValue = 0.5f;  // Snap to 0.5m for translation/scale
                // Snap to 45 degrees for rotation
                if (m_GizmoType == ImGuizmo::OPERATION::ROTATE) snapValue = 45.0f;

                float snapValues[3] = {snapValue, snapValue, snapValue};

                ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL,
                                     glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);
                if (ImGuizmo::IsUsing()) {
                    glm::vec3 translation, rotation, scale;
                    Math::DecomposeTransform(transform, translation, rotation, scale);

                    glm::vec3 deltaRotation = rotation - tc.Rotation;
                    tc.Translation = translation;
                    tc.Rotation += deltaRotation;
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
        m_CameraController.OnEvent(e);
        if (m_SceneState == SceneState::Edit) {
            m_EditorCamera.OnEvent(e);
        }
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(ET_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(ET_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
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
                m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
                m_MaterialEditPanel.SetSelectEntity(m_HoveredEntity);
            }
        }
        return false;
    }

    void EditorLayer::NewScene() {
        m_ActiveScene = Ref<Scene>::Create();
        m_ActiveScene->OnViewportResize((uint32_t)GlobalContext::GetViewportSize().x, (uint32_t)GlobalContext::GetViewportSize().y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        m_EditorScenePath = std::filesystem::path();
        m_EditorScene = m_ActiveScene;
    }

    void EditorLayer::OpenScene() {
        std::string filepath = FileDialogs::OpenFile("Scene (*.hscene)\0*.hscene\0");
        if (!filepath.empty()) {
            OpenScene(std::filesystem::path(filepath));
        }
    }

    void EditorLayer::OpenScene(const std::filesystem::path& path) {
        if (m_SceneState != SceneState::Edit) OnSceneStop();

        // TODO!: Make sure no panel is drawing entity in current scene, otherwise it will crash
        m_HoveredEntity = Entity();
        m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
        m_MaterialEditPanel.SetSelectEntity(m_HoveredEntity);

        if (path.extension().string() != ".hscene") {
            ET_WARN("Could not load {0} - not a scene file", path.filename().string());
            return;
        }
        m_EditorScene = Ref<Scene>::Create();
        m_EditorScene->OnViewportResize((uint32_t)GlobalContext::GetViewportSize().x, (uint32_t)GlobalContext::GetViewportSize().y);
        m_SceneHierarchyPanel.SetContext(m_EditorScene);

        SceneSerializer serializer(m_EditorScene);
        ET_CORE_ASSERT(serializer.Deserialize(path.string()), "Deserialize Scene {0} failed", path.string());

        m_ActiveScene = m_EditorScene;
        m_EditorScenePath = path;
    }

    void EditorLayer::SaveSceneAs() {
        std::string filepath = FileDialogs::SaveFile("Scene (*.hscene)\0*.hscene\0");
        if (!filepath.empty()) {
            SerializeScene(m_ActiveScene, filepath);
            m_EditorScenePath = filepath;
        }
    }

    void EditorLayer::SaveScene() {
        if (!m_EditorScenePath.empty())
            SerializeScene(m_ActiveScene, m_EditorScenePath);
        else
            SaveSceneAs();
    }

    void EditorLayer::DuplicateEntity() {
        if (m_SceneState != SceneState::Edit) return;

        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity) m_EditorScene->DuplicateEntity(selectedEntity);
    }

    void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path) {
        SceneSerializer serializer(scene);
        serializer.Serialize(path.string());
    }

    void EditorLayer::OnScenePlay() {
        // TODO: Save Scene when changed
        m_SceneState = SceneState::Play;
        m_ActiveScene = Scene::Copy(m_EditorScene);
        m_ActiveScene->OnRuntimeStart();
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnSceneStop() {
        m_SceneState = SceneState::Edit;
        m_ActiveScene->OnRuntimeStop();
        m_ActiveScene = m_EditorScene;
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
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
}  // namespace Ethereal