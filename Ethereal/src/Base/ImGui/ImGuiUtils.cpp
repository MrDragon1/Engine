#include "ImGuiUtils.h"

namespace Ethereal::UI {

    bool BeginPopup(const char* str_id, ImGuiWindowFlags flags)
    {
        bool opened = false;
        if (ImGui::BeginPopup(str_id, flags))
        {
            opened = true;
            // Fill background wiht nice gradient
            const float padding = ImGui::GetStyle().WindowBorderSize;
            const ImRect windowRect = UI::RectExpanded(ImGui::GetCurrentWindow()->Rect(), -padding, -padding);
            ImGui::PushClipRect(windowRect.Min, windowRect.Max, false);
            const ImColor col1 = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);// Colours::Theme::backgroundPopup;
            const ImColor col2 = UI::ColorWithMultipliedValue(col1, 0.8f);
            ImGui::GetWindowDrawList()->AddRectFilledMultiColor(windowRect.Min, windowRect.Max, col1, col1, col2, col2);
            ImGui::GetWindowDrawList()->AddRect(windowRect.Min, windowRect.Max, UI::ColorWithMultipliedValue(col1, 1.1f));
            ImGui::PopClipRect();

            // Popped in EndPopup()
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(0, 0, 0, 80));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));
        }

        return opened;
    }

    void EndPopup()
    {
        ImGui::PopStyleVar(); // WindowPadding;
        ImGui::PopStyleColor(); // HeaderHovered;
        ImGui::EndPopup();
    }

}
