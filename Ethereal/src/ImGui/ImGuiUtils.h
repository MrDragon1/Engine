#pragma once
#include <imgui_internal.h>
#include "imgui.h"

#include "Colors.h"

namespace Ethereal::UI
{
    class ScopedStyle {
      public:
        ScopedStyle(const ScopedStyle&) = delete;
        ScopedStyle& operator=(const ScopedStyle&) = delete;
        template <typename T>
        ScopedStyle(ImGuiStyleVar styleVar, T value) {
            ImGui::PushStyleVar(styleVar, value);
        }
        ~ScopedStyle() { ImGui::PopStyleVar(); }
    };

    //=========================================================================================
    /// Cursor

    static void ShiftCursorX(float distance) { ImGui::SetCursorPosX(ImGui::GetCursorPosX() + distance); }

    static void ShiftCursorY(float distance) { ImGui::SetCursorPosY(ImGui::GetCursorPosY() + distance); }

    static void ShiftCursor(float x, float y) {
        const ImVec2 cursor = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(cursor.x + x, cursor.y + y));
    }

    // TODO: move most of the functions in this header into the Draw namespace
    namespace Draw
    {
        //=========================================================================================
        /// Lines
        static void Underline(bool fullWidth = false, float offsetX = 0.0f, float offsetY = -1.0f) {
            if (fullWidth) {
                if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr)
                    ImGui::PushColumnsBackground();
                else if (ImGui::GetCurrentTable() != nullptr)
                    ImGui::TablePushBackgroundChannel();
            }

            const float width = fullWidth ? ImGui::GetWindowWidth() : ImGui::GetContentRegionAvail().x;
            const ImVec2 cursor = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x + offsetX, cursor.y + offsetY), ImVec2(cursor.x + width, cursor.y + offsetY),
                                                Colors::Theme::backgroundDark, 1.0f);

            if (fullWidth) {
                if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr)
                    ImGui::PopColumnsBackground();
                else if (ImGui::GetCurrentTable() != nullptr)
                    ImGui::TablePopBackgroundChannel();
            }
        }
    }  // namespace Draw
}  // namespace Ethereal::UI
