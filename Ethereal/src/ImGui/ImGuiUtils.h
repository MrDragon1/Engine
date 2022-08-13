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

    //=========================================================================================
    /// Rectangle

    static inline ImRect GetItemRect() { return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()); }

    static inline ImRect RectExpanded(const ImRect& rect, float x, float y) {
        ImRect result = rect;
        result.Min.x -= x;
        result.Min.y -= y;
        result.Max.x += x;
        result.Max.y += y;
        return result;
    }

    static inline ImRect RectOffset(const ImRect& rect, float x, float y) {
        ImRect result = rect;
        result.Min.x += x;
        result.Min.y += y;
        result.Max.x += x;
        result.Max.y += y;
        return result;
    }

    static inline ImRect RectOffset(const ImRect& rect, ImVec2 xy) { return RectOffset(rect, xy.x, xy.y); }

    static void DrawItemActivityOutline(float rounding = 0.0f, bool drawWhenInactive = false, ImColor colourWhenActive = ImColor(80, 80, 80)) {
        auto* drawList = ImGui::GetWindowDrawList();
        const ImRect rect = RectExpanded(GetItemRect(), 1.0f, 1.0f);
        if (ImGui::IsItemHovered() && !ImGui::IsItemActive()) {
            drawList->AddRect(rect.Min, rect.Max, ImColor(60, 60, 60), rounding, 0, 1.5f);
        }
        if (ImGui::IsItemActive()) {
            drawList->AddRect(rect.Min, rect.Max, colourWhenActive, rounding, 0, 1.0f);
        } else if (!ImGui::IsItemHovered() && drawWhenInactive) {
            drawList->AddRect(rect.Min, rect.Max, ImColor(50, 50, 50), rounding, 0, 1.0f);
        }
    };
}  // namespace Ethereal::UI
