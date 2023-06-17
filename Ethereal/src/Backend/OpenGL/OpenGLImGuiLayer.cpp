#include "OpenGLImGuiLayer.h"

#include "pch.h"
#include "Base/Application.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "Base/ImGui/ImGuizmo.h"
namespace Ethereal {

OpenGLImGuiLayer::OpenGLImGuiLayer() : ImGuiLayer() {}

OpenGLImGuiLayer::~OpenGLImGuiLayer() {}

void OpenGLImGuiLayer::OnAttach() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
                                             // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
                                             // // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform Windows
    //  io.ConfigViewportsNoAutoMerge = true; // SHOULD BE FALSE, OR IMGUIZMO WONT WORK CORRECTLY!!
    //  io.ConfigViewportsNoTaskBarIcon = true;

    io.Fonts->AddFontFromFileTTF("assets/fonts/arial/arialbd.ttf", 14.0f);
    io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/arial/arial.ttf", 14.0f);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform
    // windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    SetDarkThemeColors();

    Application& app = Application::Get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}

void OpenGLImGuiLayer::OnDetach() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void OpenGLImGuiLayer::OnImGuiRender() {
    // static bool show = true;
    // ImGui::ShowDemoWindow(&show);
    // ImGui::ShowDebugLogWindow();
}

void OpenGLImGuiLayer::Begin() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void OpenGLImGuiLayer::End() {
    ImGuiIO& io = ImGui::GetIO();
    Application& app = Application::Get();
    io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void OpenGLImGuiLayer::OnEvent(Event& e) {
    // Assign e.Handled to block this event
    //        if (mBlockEvents) {
    //            ImGuiIO& io = ImGui::GetIO();
    //            e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
    //            e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
    //        }
}

}  // namespace Ethereal
