#include "pch.h"
#include "ImGuiLayer.h"
#include "Colors.h"
#include "Backend/OpenGL/OpenGLImGuiLayer.h"
#include "Backend/Vulkan/VulkanImGuiLayer.h"

namespace Ethereal {
ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

Ref<ImGuiLayer> ImGuiLayer::Create(BackendType backend) {
    switch (backend) {
        case BackendType::NONE:
            ET_CORE_WARN("Backend is not specificated!");
        case BackendType::OPENGL:
            return Ref<OpenGLImGuiLayer>::Create();
        case BackendType::VULKAN:
            return Ref<VulkanImGuiLayer>::Create();
    }
    return Ref<OpenGLImGuiLayer>::Create();
}

void ImGuiLayer::SetDarkThemeColors() {
    auto& style = ImGui::GetStyle();
    auto& colors = ImGui::GetStyle().Colors;
    //========================================================
    /// Colors

    // Headers
    colors[ImGuiCol_Header] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::groupHeader);
    colors[ImGuiCol_HeaderHovered] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::groupHeader);
    colors[ImGuiCol_HeaderActive] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::groupHeader);

    // Buttons
    colors[ImGuiCol_Button] = ImColor(56, 56, 56, 200);
    colors[ImGuiCol_ButtonHovered] = ImColor(70, 70, 70, 255);
    colors[ImGuiCol_ButtonActive] = ImColor(56, 56, 56, 150);

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::propertyField);
    colors[ImGuiCol_FrameBgHovered] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::propertyField);
    colors[ImGuiCol_FrameBgActive] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::propertyField);

    // Tabs
    colors[ImGuiCol_Tab] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::titlebar);
    colors[ImGuiCol_TabHovered] = ImColor(60, 60, 60, 255);
    colors[ImGuiCol_TabActive] = ImColor(60, 60, 60, 255);
    colors[ImGuiCol_TabUnfocused] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::titlebar);
    colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_TabHovered];

    // Title
    colors[ImGuiCol_TitleBg] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::titlebar);
    colors[ImGuiCol_TitleBgActive] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::titlebar);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Resize Grip
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);

    // Check Mark
    colors[ImGuiCol_CheckMark] = ImColor(200, 200, 200, 255);

    // Slider
    colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);

    // Text
    colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::text);

    // Checkbox
    colors[ImGuiCol_CheckMark] = ImColor(141, 200, 164, 255);

    // Separator
    colors[ImGuiCol_Separator] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::backgroundDark);
    colors[ImGuiCol_SeparatorActive] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::highlight);
    colors[ImGuiCol_SeparatorHovered] = ImColor(39, 185, 242, 150);

    // Window Background
    colors[ImGuiCol_WindowBg] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::background);
    colors[ImGuiCol_ChildBg] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::background);
    colors[ImGuiCol_PopupBg] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::backgroundPopup);
    colors[ImGuiCol_Border] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::border);
    colors[ImGuiCol_FrameBg] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::framebg);
    colors[ImGuiCol_FrameBgHovered] = ImColor(70, 70, 70, 255);
    colors[ImGuiCol_FrameBgActive] = ImColor(70, 70, 70, 255);

    // Tables
    colors[ImGuiCol_TableHeaderBg] = ImGui::ColorConvertU32ToFloat4(Colors::Theme::groupHeader);
    colors[ImGuiCol_TableBorderLight] =
        ImGui::ColorConvertU32ToFloat4(Colors::Theme::backgroundDark);
    colors[ImGuiCol_TableRowBg] = ImVec4{0.0f, 0.0f, 0.0f, 0.0f};
    colors[ImGuiCol_TableRowBgAlt] = ImVec4{0.0f, 0.0f, 0.0f, 0.0f};
    colors[ImGuiCol_TableBorderStrong] = ImVec4{0.0f, 0.0f, 0.0f, 0.0f};

    // Menubar
    colors[ImGuiCol_MenuBarBg] = ImVec4{0.0f, 0.0f, 0.0f, 0.0f};

    //========================================================
    /// Style
    style.FrameRounding = 2.5f;
    style.FrameBorderSize = 1.0f;
    style.IndentSpacing = 11.0f;
}

}  // namespace Ethereal