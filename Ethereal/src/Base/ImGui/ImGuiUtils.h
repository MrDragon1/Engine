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

    class ScopedColor
    {
      public:
        ScopedColor(const ScopedColor&) = delete;
        ScopedColor& operator=(const ScopedColor&) = delete;
        template<typename T>
        ScopedColor(ImGuiCol ColorId, T Color) { ImGui::PushStyleColor(ColorId, Color); }
        ~ScopedColor() { ImGui::PopStyleColor(); }
    };

    class ScopedFont
    {
      public:
        ScopedFont(const ScopedFont&) = delete;
        ScopedFont& operator=(const ScopedFont&) = delete;
        ScopedFont(ImFont* font) { ImGui::PushFont(font); }
        ~ScopedFont() { ImGui::PopFont(); }
    };

    class ScopedID
    {
      public:
        ScopedID(const ScopedID&) = delete;
        ScopedID& operator=(const ScopedID&) = delete;
        template<typename T>
        ScopedID(T id) { ImGui::PushID(id); }
        ~ScopedID() { ImGui::PopID(); }
    };

    class ScopedColorStack
    {
      public:
        ScopedColorStack(const ScopedColorStack&) = delete;
        ScopedColorStack& operator=(const ScopedColorStack&) = delete;

        template <typename ColorType, typename... OtherColors>
        ScopedColorStack(ImGuiCol firstColorID, ColorType firstColor, OtherColors&& ... otherColorPairs)
            : m_Count((sizeof... (otherColorPairs) / 2) + 1)
        {
            static_assert ((sizeof... (otherColorPairs) & 1u) == 0,
                          "ScopedColorStack constructor expects a list of pairs of Color IDs and Colors as its arguments");

            PushColor(firstColorID, firstColor, std::forward<OtherColors>(otherColorPairs)...);
        }

        ~ScopedColorStack() { ImGui::PopStyleColor(m_Count); }

      private:
        int m_Count;

        template <typename ColorType, typename... OtherColors>
        void PushColor(ImGuiCol ColorID, ColorType Color, OtherColors&& ... otherColorPairs)
        {
            if constexpr (sizeof... (otherColorPairs) == 0)
            {
                ImGui::PushStyleColor(ColorID, Color);
            }
            else
            {
                ImGui::PushStyleColor(ColorID, Color);
                PushColor(std::forward<OtherColors>(otherColorPairs)...);
            }
        }
    };

    class ScopedStyleStack
    {
      public:
        ScopedStyleStack(const ScopedStyleStack&) = delete;
        ScopedStyleStack& operator=(const ScopedStyleStack&) = delete;

        template <typename ValueType, typename... OtherStylePairs>
        ScopedStyleStack(ImGuiStyleVar firstStyleVar, ValueType firstValue, OtherStylePairs&& ... otherStylePairs)
            : m_Count((sizeof... (otherStylePairs) / 2) + 1)
        {
            static_assert ((sizeof... (otherStylePairs) & 1u) == 0,
                          "ScopedStyleStack constructor expects a list of pairs of Color IDs and Colors as its arguments");

            PushStyle(firstStyleVar, firstValue, std::forward<OtherStylePairs>(otherStylePairs)...);
        }

        ~ScopedStyleStack() { ImGui::PopStyleVar(m_Count); }

      private:
        int m_Count;

        template <typename ValueType, typename... OtherStylePairs>
        void PushStyle(ImGuiStyleVar styleVar, ValueType value, OtherStylePairs&& ... otherStylePairs)
        {
            if constexpr (sizeof... (otherStylePairs) == 0)
            {
                ImGui::PushStyleVar(styleVar, value);
            }
            else
            {
                ImGui::PushStyleVar(styleVar, value);
                PushStyle(std::forward<OtherStylePairs>(otherStylePairs)...);
            }
        }
    };

    // Check if navigated to current item, e.g. with arrow keys
    static bool NavigatedTo()
    {
        ImGuiContext& g = *GImGui;
        return g.NavJustMovedToId == g.LastItemData.ID;
    }

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

    static void DrawItemActivityOutline(float rounding = 0.0f, bool drawWhenInactive = false, ImColor ColorWhenActive = ImColor(80, 80, 80)) {
        auto* drawList = ImGui::GetWindowDrawList();
        const ImRect rect = RectExpanded(GetItemRect(), 1.0f, 1.0f);
        if (ImGui::IsItemHovered() && !ImGui::IsItemActive()) {
            drawList->AddRect(rect.Min, rect.Max, ImColor(60, 60, 60), rounding, 0, 1.5f);
        }
        if (ImGui::IsItemActive()) {
            drawList->AddRect(rect.Min, rect.Max, ColorWhenActive, rounding, 0, 1.0f);
        } else if (!ImGui::IsItemHovered() && drawWhenInactive) {
            drawList->AddRect(rect.Min, rect.Max, ImColor(50, 50, 50), rounding, 0, 1.0f);
        }
    };
    
    // Colors
    static ImU32 ColorWithValue(const ImColor& color, float value)
    {
        const ImVec4& colRow = color.Value;
        float hue, sat, val;
        ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
        return ImColor::HSV(hue, sat, std::min(value, 1.0f));
    }
    
    static ImU32 ColorWithSaturation(const ImColor& color, float saturation)
    {
        const ImVec4& colRow = color.Value;
        float hue, sat, val;
        ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
        return ImColor::HSV(hue, std::min(saturation, 1.0f), val);
    }

    static ImU32 ColorWithHue(const ImColor& color, float hue)
    {
        const ImVec4& colRow = color.Value;
        float h, s, v;
        ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, h, s, v);
        return ImColor::HSV(std::min(hue, 1.0f), s, v);
    }

    static ImU32 ColorWithMultipliedValue(const ImColor& color, float multiplier)
    {
        const ImVec4& colRow = color.Value;
        float hue, sat, val;
        ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
        return ImColor::HSV(hue, sat, std::min(val * multiplier, 1.0f));
    }

    static ImU32 ColorWithMultipliedSaturation(const ImColor& color, float multiplier)
    {
        const ImVec4& colRow = color.Value;
        float hue, sat, val;
        ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
        return ImColor::HSV(hue, std::min(sat * multiplier, 1.0f), val);
    }

    static ImU32 ColorWithMultipliedHue(const ImColor& color, float multiplier)
    {
        const ImVec4& colRow = color.Value;
        float hue, sat, val;
        ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
        return ImColor::HSV(std::min(hue * multiplier, 1.0f), sat, val);
    }
    
}  // namespace Ethereal::UI
