#pragma once
#include "pch.h"
#include "ImGuiUtils.h"
#include "Core/Renderer/Texture.h"

#include "imgui.h"

namespace Ethereal
{
    namespace UI
    {
        static int s_UIContextID = 0;
        static uint32_t s_Counter = 0;
        static char s_IDBuffer[16] = "##";
        static char s_LabelIDBuffer[1024];
        static char* s_MultilineBuffer = nullptr;

        void Image(const Ref<Texture2D>& texture, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));

        static void PushID() {
            ImGui::PushID(s_UIContextID++);
            s_Counter = 0;
        }

        static void PopID() {
            ImGui::PopID();
            s_UIContextID--;
        }

        static void BeginPropertyGrid(uint32_t columns = 2) {
            PushID();
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
            ImGui::Columns(columns);
        }

        static bool PropertyGridHeader(const std::string& name, bool openByDefault = true) {
            ImGuiTreeNodeFlags treeNodeFlags =
                ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

            if (openByDefault) treeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

            bool open = false;
            const float framePaddingX = 6.0f;
            const float framePaddingY = 6.0f;  // affects height of the header

            UI::ScopedStyle headerRounding(ImGuiStyleVar_FrameRounding, 0.0f);
            UI::ScopedStyle headerPaddingAndHeight(ImGuiStyleVar_FramePadding, ImVec2{framePaddingX, framePaddingY});

            // UI::PushID();
            ImGui::PushID(name.c_str());
            open = ImGui::TreeNodeEx("##dummy_id", treeNodeFlags, name.c_str());
            // UI::PopID();
            ImGui::PopID();

            return open;
        }

        static void EndPropertyGrid() {
            ImGui::Columns(1);
            UI::Draw::Underline();
            ImGui::PopStyleVar(2);  // ItemSpacing, FramePadding
            UI::ShiftCursorY(18.0f);
            PopID();
        }

        static bool BeginTreeNode(const char* name, bool defaultOpen = true) {
            ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
            if (defaultOpen) treeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

            return ImGui::TreeNodeEx(name, treeNodeFlags);
        }

        static void EndTreeNode() { ImGui::TreePop(); }

    };  // namespace UI

}  // namespace Ethereal
