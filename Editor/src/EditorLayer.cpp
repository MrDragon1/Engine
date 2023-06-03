#include "EditorLayer.h"
#include "imgui.h"
#include "pch.h"

#include <Resource/ResourceImporter.h>

#include "Base/GlobalContext.h"
#include "Base/ImGui/ImGuizmo.h"
#include "Base/ImGui/UI.h"
#include "Base/Meta/Serializer.h"
#include "Base/Meta/_generated/serializer/all_serializer.h"
#include "Core/Asset/AssetManager.h"
#include "Core/Editor/EditorResource.h"
#include "Core/Scene/Components.h"
#include "Core/Scene/SceneSerializer.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/DebugPanel.h"
#include "Panels/MaterialEditPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels//MaterialGraphPanel.h"
#include "Utils/PlatformUtils.h"

namespace Ethereal {
#define SCENE_HIERARCHY_PANEL_ID "SceneHierarchyPanel"
#define CONTENT_BROWSER_PANEL_ID "ContentBrowserPanel"
#define MATERIALS_PANEL_ID "MaterialEditPanel"
#define DEBUG_PANEL_ID "DebugPanel"
#define MATERIAL_GRAPH_PANEL_ID "MaterialGraphPanel"

EditorLayer::EditorLayer() : Layer("EditorLayer") {}

void EditorLayer::OnAttach() {
    EditorResource::Init();
    mPanelManager = CreateScope<PanelManager>();

    mPanelManager->AddPanel<SceneHierarchyPanel>(PanelCategory::View, SCENE_HIERARCHY_PANEL_ID,
                                                 "Scene Hierarchy", true, mEditorScene,
                                                 SelectionContext::Scene);
    mPanelManager->AddPanel<ContentBrowserPanel>(PanelCategory::View, CONTENT_BROWSER_PANEL_ID,
                                                 "Content Browser", true);
    mPanelManager->AddPanel<MaterialEditPanel>(PanelCategory::View, MATERIALS_PANEL_ID, "Material",
                                               true);
    mPanelManager->AddPanel<DebugPanel>(PanelCategory::View, DEBUG_PANEL_ID, "Debug", true);
    mPanelManager->AddPanel<MaterialGraphPanel>(PanelCategory::View, MATERIAL_GRAPH_PANEL_ID,
                                                "Material Graph", true);

    mIconPlay = EditorResource::PlayIcon;
    mIconStop = EditorResource::StopIcon;

    mEditorCamera = EditorCamera(30.0f, 1280.0f / 720.0f, 0.1f, 500.0f);

    // TODO: Remove this when support default project
    OpenScene("assets/scenes/meta.EScene");
}

void EditorLayer::OnDetach() { EditorResource::Shutdown(); }

void EditorLayer::OnUpdate(TimeStamp ts) {
    mPanelManager->OnUpdate(ts);

    switch (mSceneState) {
        case SceneState::Play: {
            mRuntimeScene->OnUpdateRuntime(ts);
            break;
        }
        case SceneState::Edit: {
            mEditorCamera.OnUpdate(ts);

            SceneParam& sp = Project::GetConfigManager().sUniformManagerConfig.SceneParam;
            sp.CameraPosition = mEditorCamera.GetPosition();
            sp.ScissorNormalized =
                Vector4(1, 1, 4096 - 2, 4096 - 2) / 4096;  // the size of shadow map
            sp.EnvironmentMapIntensity = 1.0f;

            CameraParam& cp = Project::GetConfigManager().sUniformManagerConfig.MainCameraParam;
            cp.ViewProjectionMatrix = mEditorCamera.GetViewProjection();
            cp.ViewMatrix = mEditorCamera.GetViewMatrix();
            cp.ProjectionMatrix = mEditorCamera.GetProjection();
            cp.InverseViewProjectionMatrix = Math::Inverse(mEditorCamera.GetViewProjection());
            cp.InverseViewMatrix = Math::Inverse(mEditorCamera.GetViewMatrix());
            cp.InverseProjectionMatrix = Math::Inverse(mEditorCamera.GetProjection());
            cp.NearPlane = mEditorCamera.GetNearPlane();
            cp.FarPlane = mEditorCamera.GetFarPlane();

            Project::GetConfigManager().sCSMConfig.NearPlane = mEditorCamera.GetNearPlane();
            Project::GetConfigManager().sCSMConfig.FarPlane = mEditorCamera.GetFarPlane();
            Project::GetConfigManager().sCSMConfig.ViewMatrix = mEditorCamera.GetViewMatrix();
            Project::GetConfigManager().sCSMConfig.AspectRatio = mEditorCamera.GetAspectRatio();
            Project::GetConfigManager().sCSMConfig.FOV = mEditorCamera.GetFOV();

            mEditorScene->OnUpdateEditor(ts);
            break;
        }
    }
    GlobalContext::GetRenderSystem().Draw(ts);

    if (mSceneState == SceneState::Edit) {
        auto [mx, my] = ImGui::GetMousePos();
        mx -= mViewportBounds[0].x;
        my -= mViewportBounds[0].y;
        Vector2 viewportSize = mViewportBounds[1] - mViewportBounds[0];
        my = viewportSize.y - my;
        int mouseX = (int)mx;
        int mouseY = (int)my;

        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x &&
            mouseY < (int)viewportSize.y) {
            int pixelData = GlobalContext::GetRenderSystem().GetMousePicking(mouseX, mouseY);
            mHoveredEntity =
                pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, mCurrentScene.Raw());
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
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and
    // handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking,
    // otherwise any change of dockspace/settings would lead to windows being stuck in limbo and
    // never being visible.
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
            // Disabling fullscreen would allow the window to be moved to the front of other
            // windows, which we can't undo at the moment without finer window depth/z control.
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

    mPanelManager->OnImGuiRender();

    ImGui::Begin("Stats");
    std::string name = "None";
    if (mHoveredEntity) name = mHoveredEntity.GetName();
    ImGui::Text("Hovered Entity: %s", name.c_str());

    // TODO: fix this
    // const char* drawModeStrings[] = {"FILLED", "LINE", "POINT"};
    // const char* currentDrawModeString = drawModeStrings[(int)RenderCommand::GetDrawMode()];
    // if (ImGui::BeginCombo("Draw Mode", currentDrawModeString)) {
    //     for (int i = 0; i < 3; i++) {
    //         bool isSelected = currentDrawModeString == drawModeStrings[i];
    //         if (ImGui::Selectable(drawModeStrings[i], isSelected)) {
    //             currentDrawModeString = drawModeStrings[i];
    //             RenderCommand::SetDrawMode((ETHEREAL_DRAW_MODE)i);
    //         }

    //        if (isSelected) ImGui::SetItemDefaultFocus();
    //    }
    //    ImGui::EndCombo();
    //}

    ImGui::DragFloat3(
        "Directional Light Dir",
        Math::Ptr(Project::GetConfigManager().sUniformManagerConfig.LightParam.Direction), 0.01);

    // TODOLIST
    {
        UI::ScopedColorStack style(ImGuiCol_TextDisabled, IM_COL32(66, 129, 42, 255));
        ImGui::TextDisabled("TODO:");
        ImGui::TextDisabled("\tManage data to render system");
        ImGui::TextDisabled("\tSave Material at runtime");
        ImGui::TextDisabled("\tPopup selection");
    }

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Viewport");
    {
        auto viewportOffset = ImGui::GetCursorPos();  // Includes tab bar

        mViewportFocused = ImGui::IsWindowFocused();
        mViewportHovered = ImGui::IsWindowHovered();
        // Not support for now
        //            Application::Get().GetImGuiLayer()->BlockEvents(!(mViewportHovered &&
        //            Input::IsMouseButtonPressed(Mouse::ButtonRight)));

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        GlobalContext::SetViewportSize({viewportPanelSize.x, viewportPanelSize.y});
        // Resize
        if (GlobalContext::GetViewportSize().x > 0.0f &&
            GlobalContext::GetViewportSize().y > 0.0f &&  // zero sized framebuffer is invalid
            (GlobalContext::GetRenderSystem().GetMainImageWidth() !=
                 GlobalContext::GetViewportSize().x ||
             GlobalContext::GetRenderSystem().GetMainImageHeight() !=
                 GlobalContext::GetViewportSize().y)) {
            GlobalContext::GetRenderSystem().OnResize();
            // mCameraController.OnResize(GlobalContext::GetViewportSize().x,
            // GlobalContext::GetViewportSize().y);
            mEditorCamera.SetViewportSize();
            mCurrentScene->OnViewportResize((uint32_t)GlobalContext::GetViewportSize().x,
                                            (uint32_t)GlobalContext::GetViewportSize().y);
        }
        uint64_t textureID = GlobalContext::GetRenderSystem().GetMainImage();
        // ET_CORE_INFO("texture ID {}", textureID);
        ImGui::Image(reinterpret_cast<void*>(textureID),
                     ImVec2{GlobalContext::GetViewportSize().x, GlobalContext::GetViewportSize().y},
                     ImVec2{0, 1}, ImVec2{1, 0});

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload =
                    ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                AssetHandle assetHandle = *((AssetHandle*)payload->Data);
                const AssetMetaData& assetData = AssetManager::GetMetadata(assetHandle);

                std::filesystem::path workingDirPath =
                    Project::GetAssetDirectory() / assetData.FilePath;

                if (assetData.Type == AssetType::Scene)
                    OpenScene(workingDirPath.string());
                else if (assetData.Type == AssetType::StaticMesh)
                    mEditorScene->CreateEntityWithStaticMesh(assetHandle);
                else if (assetData.Type == AssetType::Mesh)
                    mEditorScene->CreateEntityWithMesh(assetHandle);
            }
            ImGui::EndDragDropTarget();
        }

        ImVec2 minBound = ImGui::GetWindowPos();
        minBound.x += viewportOffset.x;
        minBound.y += viewportOffset.y;

        ImVec2 maxBound = {minBound.x + GlobalContext::GetViewportSize().x,
                           minBound.y + GlobalContext::GetViewportSize().y};
        mViewportBounds[0] = {minBound.x, minBound.y};
        mViewportBounds[1] = {maxBound.x, maxBound.y};

        // Gizmos
        Entity selectedEntity = Entity();
        if (SelectionManager::GetSelections(SelectionContext::Scene).size() > 0)
            selectedEntity = mCurrentScene->GetEntityWithUUID(
                SelectionManager::GetSelections(SelectionContext::Scene)[0]);
        if (selectedEntity && mGizmoType != -1) {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            float windowWidth = (float)ImGui::GetWindowWidth();
            float windowHeight = (float)ImGui::GetWindowHeight();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth,
                              windowHeight);

            // Editor camera
            const Matrix4& cameraProjection = mEditorCamera.GetProjection();
            Matrix4 cameraView = mEditorCamera.GetViewMatrix();

            // Entity transform
            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            Matrix4 transform = tc.getMatrix();

            // Snapping
            bool snap = Input::IsKeyPressed(Key::LeftControl);
            float snapValue = 0.5f;  // Snap to 0.5m for translation/scale
            // Snap to 45 degrees for rotation
            if (mGizmoType == ImGuizmo::OPERATION::ROTATE) snapValue = 45.0f;

            float snapValues[3] = {snapValue, snapValue, snapValue};

            ImGuizmo::Manipulate(Math::Ptr(cameraView), Math::Ptr(cameraProjection),
                                 (ImGuizmo::OPERATION)mGizmoType, ImGuizmo::LOCAL,
                                 Math::Ptr(transform), nullptr, snap ? snapValues : nullptr);
            if (ImGuizmo::IsUsing()) {
                Vector3 translation, scale, skew;
                Vector4 perspective;
                Quaternion rotation;

                Math::DecomposeTransformMatrix(transform, translation, rotation, scale, skew,
                                               perspective);

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
    if (mSceneState == SceneState::Edit && mViewportHovered) {
        mEditorCamera.OnEvent(e);
    }
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<KeyPressedEvent>(ET_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
    dispatcher.Dispatch<MouseButtonPressedEvent>(
        ET_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    mPanelManager->OnEvent(e);
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
        // TODO: Find a more human friendly way bind keys
        case Key::C:
            mGizmoType = -1;
            break;
        case Key::R:
            mGizmoType = ImGuizmo::OPERATION::TRANSLATE;
            break;
        case Key::F:
            mGizmoType = ImGuizmo::OPERATION::ROTATE;
            break;
        case Key::V:
            mGizmoType = ImGuizmo::OPERATION::SCALE;
            break;
    }
    return true;
}

bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
    if (e.GetMouseButton() == Mouse::ButtonLeft) {
        if (mViewportHovered && !ImGuizmo::IsOver()) {
            SelectionManager::DeselectAll();
            if (mHoveredEntity)
                SelectionManager::Select(SelectionContext::Scene, mHoveredEntity.GetUUID());
        }
    }
    return false;
}

void EditorLayer::NewScene() {
    mEditorScene = Ref<Scene>::Create();
    mEditorScene->OnViewportResize((uint32_t)GlobalContext::GetViewportSize().x,
                                   (uint32_t)GlobalContext::GetViewportSize().y);
    mPanelManager->SetSceneContext(mEditorScene);

    mEditorScenePath = std::filesystem::path();

    mCurrentScene = mEditorScene;
}

void EditorLayer::OpenScene() {
    std::string filepath = FileDialogs::OpenFile("Scene (*.EScene)\0*.EScene\0");
    if (!filepath.empty()) {
        OpenScene(std::filesystem::path(filepath));
    }
}

void EditorLayer::OpenScene(const std::filesystem::path& path) {
    if (mSceneState != SceneState::Edit) OnSceneStop();

    //        // TODO!: Make sure no panel is drawing entity in current scene, otherwise it will
    //        crash mHoveredEntity = Entity();
    //        mSceneHierarchyPanel.SetSelectedEntity(mHoveredEntity);
    //        mMaterialEditPanel.SetSelectEntity(mHoveredEntity);

    if (AssetManager::GetAssetTypeFromExtension(path.extension().string()) != AssetType::Scene) {
        ET_WARN("Could not load {0} - not a scene file", path.filename().string());
        return;
    }
    mEditorScene = Ref<Scene>::Create();
    mEditorScene->OnViewportResize((uint32_t)GlobalContext::GetViewportSize().x,
                                   (uint32_t)GlobalContext::GetViewportSize().y);

    mPanelManager->SetSceneContext(mEditorScene);

    SceneSerializer serializer(mEditorScene);
    ET_CORE_ASSERT(serializer.Deserialize(path.string()), "Deserialize Scene {0} failed",
                   path.string());

    mCurrentScene = mEditorScene;
    mEditorScenePath = path;
}

void EditorLayer::SaveSceneAs() {
    std::string filepath = FileDialogs::SaveFile("Scene (*.EScene)\0*.EScene\0");
    if (!filepath.empty()) {
        SerializeScene(mEditorScene, filepath);
        mEditorScenePath = filepath;
    }
}

void EditorLayer::SaveScene() {
    if (!mEditorScenePath.empty())
        SerializeScene(mEditorScene, mEditorScenePath);
    else
        SaveSceneAs();
}

void EditorLayer::DuplicateEntity() {
    if (mSceneState != SceneState::Edit) return;

    Entity selectedEntity;
    if (SelectionManager::GetSelections(SelectionContext::Scene).size() > 0) {
        selectedEntity = mEditorScene->GetEntityWithUUID(
            SelectionManager::GetSelections(SelectionContext::Scene)[0]);
    }
    if (selectedEntity) mEditorScene->DuplicateEntity(selectedEntity);
}

void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path) {
    SceneSerializer serializer(scene);
    serializer.Serialize(path.string());
}

void EditorLayer::OnScenePlay() {
    SelectionManager::DeselectAll();

    // TODO: Save Scene when changed
    mSceneState = SceneState::Play;
    mRuntimeScene = Scene::Copy(mEditorScene);
    mRuntimeScene->OnRuntimeStart();

    mPanelManager->SetSceneContext(mRuntimeScene);
    mCurrentScene = mRuntimeScene;
}

void EditorLayer::OnSceneStop() {
    mSceneState = SceneState::Edit;
    mRuntimeScene->OnRuntimeStop();
    mRuntimeScene = nullptr;

    mPanelManager->SetSceneContext(mEditorScene);
    mCurrentScene = mEditorScene;
}

void EditorLayer::UI_Toolbar() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    auto& color = ImGui::GetStyle().Colors;
    const auto& buttonHovered = color[ImGuiCol_ButtonHovered];
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
    const auto& buttonActive = color[ImGuiCol_ButtonActive];
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

    ImGui::Begin("##toolbar", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse);

    float size = ImGui::GetWindowHeight() - 4.0f;
    Ref<Texture> icon = mSceneState == SceneState::Edit ? mIconPlay : mIconStop;
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

    auto api = GlobalContext::GetDriverApi();
    if (ImGui::ImageButton((ImTextureID)(intptr_t)(api->GetTextueID(icon)), ImVec2(size, size),
                           ImVec2(0, 0), ImVec2(1, 1), 0)) {
        if (mSceneState == SceneState::Edit) {
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
                     ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
        ImGui::Separator();
        ImGui::Image(reinterpret_cast<void*>(GlobalContext::GetRenderSystem().GetSkyboxImage()),
                     ImVec2(256, 128), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Separator();
        if (ImGui::Button("Load Skybox")) {
            std::string filepath = FileDialogs::OpenFile("Skybox (*.hdr)\0*.hdr\0");
            if (!filepath.empty()) {
                if (mEditorScene) {
                    ET_CORE_INFO("Open Skybox {0}", filepath);
                    mEditorScene->SetEnvironment(AssetManager::GetAsset<Environment>(filepath));
                }
            }
        }
        ImGui::Separator();
    }

    ImGui::End();
}
void EditorLayer::ShowProjectSettingWindow(bool* p_open) {
    auto& bloomSetting = Project::GetConfigManager().sBloomConfig;
    if (ImGui::Begin("Project Settings", p_open, ImGuiWindowFlags_NoCollapse)) {
        if (UI::ComponentHeader("Bloom")) {
            UI::ShiftCursorY(4.0f);
            UI::BeginPropertyGrid();

            ImGui::Text("Enabled");
            ImGui::NextColumn();
            ImGui::Checkbox("##BloomEnable", &bloomSetting.Enabled);
            ImGui::NextColumn();

            ImGui::Text("Threshold");
            ImGui::NextColumn();
            ImGui::DragFloat("##BloomThreshold", &bloomSetting.Threshold, 0.01f);
            ImGui::NextColumn();

            ImGui::Text("Knee");
            ImGui::NextColumn();
            ImGui::DragFloat("##BloomKnee", &bloomSetting.Knee, 0.01f);
            ImGui::NextColumn();

            ImGui::Text("Intensity");
            ImGui::NextColumn();
            ImGui::DragFloat("##BloomIntensity", &bloomSetting.Intensity, 0.01f);

            UI::EndPropertyGrid();
        }
        auto& fogSetting = Project::GetConfigManager().sFogConfig;

        if (UI::ComponentHeader("Fog")) {
            UI::ShiftCursorY(4.0f);
            UI::BeginPropertyGrid();
            ImGui::Text("Enable Fog");
            ImGui::NextColumn();
            ImGui::Checkbox("##FogEnable", &fogSetting.FogEnable);
            ImGui::NextColumn();

            ImGui::Text("Start");
            ImGui::NextColumn();
            ImGui::DragFloat("##FogStart", &fogSetting.FogStart, 0.1f, 0.0f, 100.0f);
            ImGui::NextColumn();

            ImGui::Text("Density");
            ImGui::NextColumn();
            ImGui::DragFloat("##FogDensity", &fogSetting.FogDensity, 0.01f, 0.0f, 1.0f);
            ImGui::NextColumn();

            ImGui::Text("Height");
            ImGui::NextColumn();
            ImGui::DragFloat("##FogHeight", &fogSetting.FogHeight, 0.1f, 0.0f, 100.0f);
            ImGui::NextColumn();

            ImGui::Text("Height falloff");
            ImGui::NextColumn();
            ImGui::DragFloat("##FogHeightFalloff", &fogSetting.FogHeightFalloff, 0.001f, 0.0f,
                             10.0f);
            ImGui::NextColumn();

            ImGui::Text("Scattering start");
            ImGui::NextColumn();
            ImGui::DragFloat("##FogScatteringStart", &fogSetting.FogScatteringStart, 0.1f, 0.0f,
                             100.0f);
            ImGui::NextColumn();

            ImGui::Text("Scattering size");
            ImGui::NextColumn();
            ImGui::DragFloat("##FogScatteringSize", &fogSetting.FogScatteringSize, 0.1f, 0.1f,
                             100.0f);
            ImGui::NextColumn();

            ImGui::Text("Color from IBL");
            ImGui::NextColumn();
            ImGui::Checkbox("##FogColorFromIBL", &fogSetting.FogFromIBL);
            ImGui::NextColumn();

            ImGui::Text("Color");
            ImGui::NextColumn();
            ImGui::ColorEdit4("##FogColor", Math::Ptr(fogSetting.FogColor));
            ImGui::NextColumn();

            UI::EndPropertyGrid();
        }
    }

    ImGui::End();
}
}  // namespace Ethereal