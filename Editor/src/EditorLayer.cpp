#include "EditorLayer.hpp"

// Should remove this
#include "Engine/Scene/Components.hpp"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/Scene/SceneSerializer.hpp"
#include "Engine/Utils/PlatformUtils.hpp"
namespace Engine
{
    EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f) {}

    void EditorLayer::OnAttach() {
        m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");

        FramebufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_Scene = CreateRef<Scene>();
#if 0
        auto square = m_Scene->CreateEntity("Green Square");
        square.AddComponent<SpriteRendererComponent>(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        m_SquareEntity = square;

        auto redSquare = m_Scene->CreateEntity("Red Square");
        redSquare.AddComponent<SpriteRendererComponent>(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});

        m_CameraEntity = m_Scene->CreateEntity("Camera Entity");
        m_CameraEntity.AddComponent<CameraComponent>();

        m_SecondCamera = m_Scene->CreateEntity("Clip-Space Entity");
        auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
        cc.Primary = false;

        class CameraController : public ScriptableEntity {
          public:
            void OnCreate() {
                auto& translation = GetComponent<TransformComponent>().Translation;
                translation.x = rand() % 10 - 5.0f;
            }

            void OnDestroy() {}

            void OnUpdate(Timestep ts) {
                auto& translation = GetComponent<TransformComponent>().Translation;
                float speed = 5.0f;

                if (Input::IsKeyPressed(KeyCode::A)) translation.x -= speed * ts;
                if (Input::IsKeyPressed(KeyCode::D)) translation.x += speed * ts;
                if (Input::IsKeyPressed(KeyCode::W)) translation.y += speed * ts;
                if (Input::IsKeyPressed(KeyCode::S)) translation.y -= speed * ts;
            }
        };

        m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif
        m_SceneHierarchyPanel.SetContext(m_Scene);
    }

    void EditorLayer::OnDetach() {}

    void EditorLayer::OnUpdate(Timestep ts) {
        // Update
        // if (m_ViewportFocused) {
        //     m_CameraController.OnUpdate(ts);
        // }

        // Render
        Renderer2D::ResetStats();

        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        RenderCommand::Clear();

        m_Scene->OnUpdate(ts);

        m_Framebuffer->Unbind();
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
        style.WindowMinSize.x = 370.0f;

        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        style.WindowMinSize.x = minWinSizeX;

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                // ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

                if (ImGui::MenuItem("New", "Ctrl+N")) NewScene();
                if (ImGui::MenuItem("Open...", "Ctrl+O")) OpenScene();
                if (ImGui::MenuItem("SaveAs...", "Ctrl+shift+S")) SaveSceneAs();

                if (ImGui::MenuItem("Exit")) Engine::Application::Get().Close();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        m_SceneHierarchyPanel.OnImguiRender();

        ImGui::Begin("Stats");

        auto stats = Engine::Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize)) {
            m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
            m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};

            m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);

            m_Scene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }
        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::End();
    }

    void EditorLayer::OnEvent(Event& e) {
        m_CameraController.OnEvent(e);
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(ENGINE_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e) {
        // Shortcuts
        if (e.GetRepeatCount() > 0) return false;

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
        switch (e.GetKeyCode()) {
            case Key::N: {
                if (control) {
                    NewScene();
                    ENGINE_CORE_INFO("call ctrl n");
                }
                break;
            }
            case Key::O: {
                if (control) OpenScene();
                break;
            }
            case Key::S: {
                if (control && shift) SaveSceneAs();
                break;
            }
        }
        return true;
    }

    void EditorLayer::NewScene() {
        m_Scene = CreateRef<Scene>();
        m_Scene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_Scene);
    }

    void EditorLayer::OpenScene() {
        std::string filepath = FileDialogs::OpenFile("Scene (*.Scene)\0*.Scene\0");
        if (!filepath.empty()) {
            m_Scene = CreateRef<Scene>();
            m_Scene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_SceneHierarchyPanel.SetContext(m_Scene);

            SceneSerializer serializer(m_Scene);
            serializer.Deserialize(filepath);
        }
    }

    void EditorLayer::SaveSceneAs() {
        std::string filepath = FileDialogs::SaveFile("Scene (*.Scene)\0*.Scene\0");
        if (!filepath.empty()) {
            SceneSerializer serializer(m_Scene);
            serializer.Serialize(filepath);
        }
    }
}  // namespace Engine