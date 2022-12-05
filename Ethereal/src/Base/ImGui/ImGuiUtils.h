#pragma once
#include <imgui_internal.h>
#include <Core/Renderer/Texture.h>
#include "imgui.h"

#include "Colors.h"

namespace Ethereal::UI
{
    //=========================================================================================
    /// Window

    bool BeginPopup(const char* str_id, ImGuiWindowFlags flags = 0);
    void EndPopup();


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

    static const char* PatchFormatStringFloatToInt(const char* fmt)
    {
        if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
            return "%d";
        const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
        const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
        if (fmt_end > fmt_start && fmt_end[-1] == 'f')
        {
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
            if (fmt_start == fmt && fmt_end[0] == 0)
                return "%d";
            ImGuiContext& g = *GImGui;
            ImFormatString(g.TempBuffer.Data, IM_ARRAYSIZE(g.TempBuffer.Data), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
            return g.TempBuffer.Data;
#else
            IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
        }
        return fmt;
    }

    static int FormatString(char* buf, size_t buf_size, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
#ifdef IMGUI_USE_STB_SPRINTF
        int w = stbsp_vsnprintf(buf, (int)buf_size, fmt, args);
#else
        int w = vsnprintf(buf, buf_size, fmt, args);
#endif
        va_end(args);
        if (buf == NULL)
            return w;
        if (w == -1 || w >= (int)buf_size)
            w = (int)buf_size - 1;
        buf[w] = 0;
        return w;
    }

    static bool DragScalar(const char* label, ImGuiDataType data_type, void* p_data, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const float w = ImGui::CalcItemWidth();

        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
        const ImRect frame_bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + w, window->DC.CursorPos.y + (label_size.y + style.FramePadding.y * 2.0f)));
        const ImRect total_bb(frame_bb.Min, ImVec2(frame_bb.Max.x + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), frame_bb.Max.y));

        const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
        ImGui::ItemSize(total_bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
            return false;

        // Default format string when passing NULL
        if (format == NULL)
            format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
        else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
            format = PatchFormatStringFloatToInt(format);

        // Tabbing or CTRL-clicking on Drag turns it into an InputText
        const bool hovered = ImGui::ItemHoverable(frame_bb, id);
        bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);
        if (!temp_input_is_active)
        {
            const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
            const bool clicked = (hovered && g.IO.MouseClicked[0]);
            const bool double_clicked = (hovered && g.IO.MouseClickedCount[0] == 2);
            if (input_requested_by_tabbing || clicked || double_clicked || g.NavActivateId == id || g.NavActivateInputId == id)
            {
                ImGui::SetActiveID(id, window);
                ImGui::SetFocusID(id, window);
                ImGui::FocusWindow(window);
                g.ActiveIdUsingNavDirMask = (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
                if (temp_input_allowed)
                    if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || double_clicked || g.NavActivateInputId == id)
                        temp_input_is_active = true;
            }

            // Experimental: simple click (without moving) turns Drag into an InputText
            if (g.IO.ConfigDragClickToInputText && temp_input_allowed && !temp_input_is_active)
                if (g.ActiveId == id && hovered && g.IO.MouseReleased[0] && !ImGui::IsMouseDragPastThreshold(0, g.IO.MouseDragThreshold * 0.5f))
                {
                    g.NavActivateId = g.NavActivateInputId = id;
                    g.NavActivateFlags = ImGuiActivateFlags_PreferInput;
                    temp_input_is_active = true;
                }
        }

        if (temp_input_is_active)
        {
            // Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
            const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0 && (p_min == NULL || p_max == NULL || ImGui::DataTypeCompare(data_type, p_min, p_max) < 0);
            return ImGui::TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
        }

        // Draw frame
        const ImU32 frame_col = ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
        ImGui::RenderNavHighlight(frame_bb, id);
        ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding);

        // Drag behavior
        const bool value_changed = ImGui::DragBehavior(id, data_type, p_data, v_speed, p_min, p_max, format, flags);
        if (value_changed)
            ImGui::MarkItemEdited(id);

        const bool mixed_value = (g.CurrentItemFlags & ImGuiItemFlags_MixedValue) != 0;

        // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
        char value_buf[64];
        const char* value_buf_end = value_buf + (mixed_value ? FormatString(value_buf, IM_ARRAYSIZE(value_buf), "---") : ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format));
        if (g.LogEnabled)
            ImGui::LogSetNextTextDecoration("{", "}");
        ImGui::RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

        if (label_size.x > 0.0f)
            ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
        return value_changed;
    }

    static bool DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0)
    {
        return DragScalar(label, ImGuiDataType_Float, v, v_speed, &v_min, &v_max, format, flags);
    }

    static bool IsItemDisabled()
    {
        return ImGui::GetItemFlags() & ImGuiItemFlags_Disabled;
    }

    //=========================================================================================
    /// Button Image

    static void DrawButtonImage(const Ref<Texture2D>& imageNormal, const Ref<Texture2D>& imageHovered, const Ref<Texture2D>& imagePressed,
                                ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
                                ImVec2 rectMin, ImVec2 rectMax)
    {
        auto* drawList = ImGui::GetWindowDrawList();
        if (ImGui::IsItemActive())
            drawList->AddImage(reinterpret_cast<ImTextureID>(imagePressed->GetRendererID()), rectMin, rectMax, ImVec2(0, 0), ImVec2(1, 1), tintPressed);
        else if (ImGui::IsItemHovered())
            drawList->AddImage(reinterpret_cast<ImTextureID>(imageHovered->GetRendererID()), rectMin, rectMax, ImVec2(0, 0), ImVec2(1, 1), tintHovered);
        else
            drawList->AddImage(reinterpret_cast<ImTextureID>(imageNormal->GetRendererID()), rectMin, rectMax, ImVec2(0, 0), ImVec2(1, 1), tintNormal);
    };

    static void DrawButtonImage(const Ref<Texture2D>& imageNormal, const Ref<Texture2D>& imageHovered, const Ref<Texture2D>& imagePressed,
                                ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
                                ImRect rectangle)
    {
        DrawButtonImage(imageNormal, imageHovered, imagePressed, tintNormal, tintHovered, tintPressed, rectangle.Min, rectangle.Max);
    };

    static void DrawButtonImage(const Ref<Texture2D>& image,
                                ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
                                ImVec2 rectMin, ImVec2 rectMax)
    {
        DrawButtonImage(image, image, image, tintNormal, tintHovered, tintPressed, rectMin, rectMax);
    };

    static void DrawButtonImage(const Ref<Texture2D>& image,
                                ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
                                ImRect rectangle)
    {
        DrawButtonImage(image, image, image, tintNormal, tintHovered, tintPressed, rectangle.Min, rectangle.Max);
    };


    static void DrawButtonImage(const Ref<Texture2D>& imageNormal, const Ref<Texture2D>& imageHovered, const Ref<Texture2D>& imagePressed,
                                ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed)
    {
        DrawButtonImage(imageNormal, imageHovered, imagePressed, tintNormal, tintHovered, tintPressed, ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    };

    static void DrawButtonImage(const Ref<Texture2D>& image,
                                ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed)
    {
        DrawButtonImage(image, image, image, tintNormal, tintHovered, tintPressed, ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    };
}  // namespace Ethereal::UI
