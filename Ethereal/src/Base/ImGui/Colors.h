#pragma once
#include <imgui.h>

namespace Colors
{
    static inline float Convert_sRGB_FromLinear(float theLinearValue);
    static inline float Convert_sRGB_ToLinear(float thesRGBValue);
    ImVec4 ConvertFromSRGB(ImVec4 colour);
    ImVec4 ConvertToSRGB(ImVec4 colour);

    // To experiment with editor theme live you can change these constexpr into static
    // members of a static "Theme" class and add a quick ImGui window to adjust the colour values
    namespace Theme
    {
        constexpr auto highlight = IM_COL32(39, 185, 242, 255);

        constexpr auto background = IM_COL32(56, 56, 56, 255);
        constexpr auto backgroundDark = IM_COL32(26, 26, 26, 255);
        constexpr auto titlebar = IM_COL32(56, 56, 56, 255);
        constexpr auto propertyField = IM_COL32(15, 15, 15, 255);
        constexpr auto text = IM_COL32(192, 192, 192, 255);
        constexpr auto framebg = IM_COL32(42, 42, 42, 255); // editable text
        constexpr auto border = IM_COL32(0,0,0, 170);

        constexpr auto groupHeader = IM_COL32(69, 69, 69, 255);
        constexpr auto backgroundPopup = IM_COL32(63, 77, 76, 255); // most green
    }                                                                // namespace Theme
}  // namespace Colors
