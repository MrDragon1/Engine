#pragma once

#include "Base/Ref.h"
#include "Core/Editor/PanelManager.h"
#include "Core/Renderer/EditorCamera.h"
#include "Ethereal.h"

namespace Ethereal {
class EditorLayer : public Layer {
   public:
    EditorLayer();
    ~EditorLayer() override = default;

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(TimeStamp ts) override;
    void OnImGuiRender() override;
    void OnEvent(Event& e) override;

   private:
    bool OnKeyPressed(KeyPressedEvent& e);
    bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

    void NewScene();
    void OpenScene();
    void OpenScene(const std::filesystem::path& path);
    void SaveSceneAs();
    void SaveScene();

    void DuplicateEntity();
    void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

    void OnScenePlay();
    void OnSceneStop();

    void UI_Toolbar();
    void ShowSkyboxSettingWindow(bool* p_open);
    void ShowProjectSettingWindow(bool* p_open);

   private:
    EditorCamera mEditorCamera;

    Scope<PanelManager> mPanelManager;

    Ref<Texture> mIconPlay;
    Ref<Texture> mIconStop;

    Ref<Scene> mRuntimeScene, mEditorScene, mCurrentScene;

    std::filesystem::path mEditorScenePath;

    Entity mHoveredEntity;

    Vector2 mViewportBounds[2];

    bool mViewportFocused = false, mViewportHovered = false;

    int mGizmoType = -1;

    enum class SceneState { Edit = 0, Play = 1 };

    SceneState mSceneState = SceneState::Edit;
    bool b_ShowDemoImGui = false;
    bool b_ShowSkyboxSettings = false;
    bool b_ShowProjectSettings = true;
    Vector3 mLightPos = Vector3(-20, 40, -10);  // as far as possiable (away from the scene to avoid clamp shadow)
};
}  // namespace Ethereal