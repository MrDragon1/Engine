#pragma once
#include <Core/Asset/AssetManager.h>
#include <Core/Editor/EditorResource.h>

#include "Base/GlobalContext.h"
#include "Colors.h"
#include "Utils/StringUtils.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "pch.h"
namespace Ethereal {
namespace UI {
static int sUIContextID = 0;
static uint32_t sCounter = 0;
static char sIDBuffer[16] = "##";
static char sLabelIDBuffer[1024];

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

class ScopedColor {
   public:
    ScopedColor(const ScopedColor&) = delete;
    ScopedColor& operator=(const ScopedColor&) = delete;
    template <typename T>
    ScopedColor(ImGuiCol ColorId, T Color) {
        ImGui::PushStyleColor(ColorId, Color);
    }
    ~ScopedColor() { ImGui::PopStyleColor(); }
};

class ScopedFont {
   public:
    ScopedFont(const ScopedFont&) = delete;
    ScopedFont& operator=(const ScopedFont&) = delete;
    ScopedFont(ImFont* font) { ImGui::PushFont(font); }
    ~ScopedFont() { ImGui::PopFont(); }
};

class ScopedID {
   public:
    ScopedID(const ScopedID&) = delete;
    ScopedID& operator=(const ScopedID&) = delete;
    template <typename T>
    ScopedID(T id) {
        ImGui::PushID(id);
    }
    ~ScopedID() { ImGui::PopID(); }
};

class ScopedColorStack {
   public:
    ScopedColorStack(const ScopedColorStack&) = delete;
    ScopedColorStack& operator=(const ScopedColorStack&) = delete;

    template <typename ColorType, typename... OtherColors>
    ScopedColorStack(ImGuiCol firstColorID, ColorType firstColor, OtherColors&&... otherColorPairs)
        : mCount((sizeof...(otherColorPairs) / 2) + 1) {
        static_assert((sizeof...(otherColorPairs) & 1u) == 0,
                      "ScopedColorStack constructor expects a list of pairs of Color IDs and "
                      "Colors as its arguments");

        PushColor(firstColorID, firstColor, std::forward<OtherColors>(otherColorPairs)...);
    }

    ~ScopedColorStack() { ImGui::PopStyleColor(mCount); }

   private:
    int mCount;

    template <typename ColorType, typename... OtherColors>
    static void PushColor(ImGuiCol ColorID, ColorType Color, OtherColors&&... otherColorPairs) {
        if constexpr (sizeof...(otherColorPairs) == 0) {
            ImGui::PushStyleColor(ColorID, Color);
        } else {
            ImGui::PushStyleColor(ColorID, Color);
            PushColor(std::forward<OtherColors>(otherColorPairs)...);
        }
    }
};

class ScopedStyleStack {
   public:
    ScopedStyleStack(const ScopedStyleStack&) = delete;
    ScopedStyleStack& operator=(const ScopedStyleStack&) = delete;

    template <typename ValueType, typename... OtherStylePairs>
    ScopedStyleStack(ImGuiStyleVar firstStyleVar, ValueType firstValue,
                     OtherStylePairs&&... otherStylePairs)
        : mCount((sizeof...(otherStylePairs) / 2) + 1) {
        static_assert((sizeof...(otherStylePairs) & 1u) == 0,
                      "ScopedStyleStack constructor expects a list of pairs of Color IDs and "
                      "Colors as its arguments");

        PushStyle(firstStyleVar, firstValue, std::forward<OtherStylePairs>(otherStylePairs)...);
    }

    ~ScopedStyleStack() { ImGui::PopStyleVar(mCount); }

   private:
    int mCount;

    template <typename ValueType, typename... OtherStylePairs>
    static void PushStyle(ImGuiStyleVar styleVar, ValueType value,
                          OtherStylePairs&&... otherStylePairs) {
        if constexpr (sizeof...(otherStylePairs) == 0) {
            ImGui::PushStyleVar(styleVar, value);
        } else {
            ImGui::PushStyleVar(styleVar, value);
            PushStyle(std::forward<OtherStylePairs>(otherStylePairs)...);
        }
    }
};

// Low Level
static void ShiftCursorX(float distance) {
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + distance);
}

static void ShiftCursorY(float distance) {
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + distance);
}

static void ShiftCursor(float x, float y) {
    const ImVec2 cursor = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(cursor.x + x, cursor.y + y));
}

static void PushID() {
    ImGui::PushID(sUIContextID++);
    sCounter = 0;
}

static void PopID() {
    ImGui::PopID();
    sUIContextID--;
}

static const char* GenerateLabelID(std::string_view label) {
    *fmt::format_to_n(sLabelIDBuffer, std::size(sLabelIDBuffer), "{}##{}", label, sCounter++).out =
        0;
    return sLabelIDBuffer;
}

static const char* GenerateID() {
    _itoa_s(sCounter++, sIDBuffer + 2, 16, 10);
    return sIDBuffer;
}

static void BeginPropertyGrid(uint32_t columns = 2) {
    PushID();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20.0f, 8.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
    ImGui::Columns(columns);
}

static void EndPropertyGrid() {
    ImGui::Columns(1);
    ImGui::PopStyleVar(2);  // ItemSpacing, FramePadding
    UI::ShiftCursorY(18.0f);
    PopID();
}

// Utils
static void DrawItemActivityOutline() {
    auto* drawList = ImGui::GetWindowDrawList();
    ImRect rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    rect.Min.x -= 1.0f;
    rect.Min.y -= 1.0f;
    rect.Max.x += 1.0f;
    rect.Max.y += 1.0f;

    if (ImGui::IsItemHovered() && !ImGui::IsItemActive()) {
        drawList->AddRect(rect.Min, rect.Max, ImColor(101, 101, 101), 2.0f, 0, 2.0f);
    }
    if (ImGui::IsItemActive()) {
        drawList->AddRect(rect.Min, rect.Max, ImColor(58, 121, 187), 2.0f, 0, 2.0f);
    }
}

static bool DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f,
                      float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
    bool res = ImGui::DragFloat(label, v, v_speed, v_min, v_max, format);
    UI::DrawItemActivityOutline();

    return res;
}

static bool ImageButton(const char* label, ImTextureID user_texture_id, const ImVec2& size,
                        ImU32 tintNormal = ImU32(IM_COL32(196, 196, 196, 255)),
                        ImU32 tintHovered = ImU32(IM_COL32(255, 255, 255, 255)),
                        ImU32 tintPressed = ImU32(IM_COL32(255, 255, 255, 255))) {
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems) return false;

    // Default to using texture ID as ID. User can still push string/integer prefixes.

    ImGuiID id = window->GetID(label);

    //            {
    //                ScopedID scopedID((void*)(intptr_t)user_texture_id);
    //                id = window->GetID("#image");
    //            }

    PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2((float)0, (float)0));
    bool pressed = false;
    const ImVec2 padding = g.Style.FramePadding;
    const ImRect bb(window->DC.CursorPos, {window->DC.CursorPos.x + size.x + padding.x * 2,
                                           window->DC.CursorPos.y + size.y + padding.y * 2});
    ItemSize(bb);
    if (ItemAdd(bb, id)) {
        bool hovered, held;
        pressed = ButtonBehavior(bb, id, &hovered, &held);

        // Render
        const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive
                                      : hovered         ? ImGuiCol_ButtonHovered
                                                        : ImGuiCol_Button);
        RenderNavHighlight(bb, id);
        RenderFrame(bb.Min, bb.Max, col, true,
                    ImClamp((float)ImMin(padding.x, padding.y), 0.0f, g.Style.FrameRounding));

        ImVec2 rectMin{bb.Min.x + padding.x, bb.Min.y + padding.y};
        ImVec2 rectMax{bb.Max.x - padding.x, bb.Max.y - padding.y};
        //                window->DrawList->AddRectFilled(rectMin, rectMax,
        //                ImU32(IM_COL32(255,0,0,100))); window->DrawList->AddImage(user_texture_id,
        //                rectMin, rectMax, ImVec2(0, 0), ImVec2(1, 1), tintNormal);
        if (ImGui::IsItemActive())
            window->DrawList->AddImage(user_texture_id, rectMin, rectMax, ImVec2(0, 1),
                                       ImVec2(1, 0), tintPressed);
        else if (ImGui::IsItemHovered())
            window->DrawList->AddImage(user_texture_id, rectMin, rectMax, ImVec2(0, 1),
                                       ImVec2(1, 0), tintHovered);
        else
            window->DrawList->AddImage(user_texture_id, rectMin, rectMax, ImVec2(0, 1),
                                       ImVec2(1, 0), tintNormal);
    }
    PopStyleVar();

    return pressed;
}

static void DrawVec3Slider(const std::string& label, Vector3& values,
                           Vector3 resetValues = Vector3(0.0f)) {
    ImGui::TableSetColumnIndex(0);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 17.0f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7.0f);
    ImGui::Text("%s", label.c_str());

    // Underline

    ImGui::TableSetColumnIndex(1);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7.0f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 0.0f);

    const float spacingX = 4.0f;
    const float spacing = 4.0f;  // spacing between the button and float slider
    const float lineHeight = ImGui::GetTextLineHeightWithSpacing();
    const ImVec2 buttonSize = {lineHeight, lineHeight};
    const float inputItemWidth =
        (ImGui::GetContentRegionAvail().x - spacingX) / 3.0f - buttonSize.x - spacing * 2.0f;

    ScopedStyleStack style(ImGuiStyleVar_ItemSpacing, ImVec2{spacingX, 0.0f},
                           ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 2.0f});
    {
        ScopedColorStack style(ImGuiCol_Border, IM_COL32(0, 0, 0, 0), ImGuiCol_FrameBg,
                               IM_COL32(0, 0, 0, 0));
        ImGui::BeginChild(ImGui::GetID((label + "fr").c_str()),
                          ImVec2(ImGui::GetContentRegionAvail().x - spacingX,
                                 ImGui::GetFrameHeightWithSpacing() + 8.0f),
                          false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }
    const ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[1];

    auto drawControl = [&](const std::string& label, float& value, float resetValue,
                           const ImVec4& colorN, const ImVec4& colorH, const ImVec4& colorP) {
        {
            ScopedStyle style(ImGuiStyleVar_FrameRounding, 1.0f);
            ScopedColorStack colorStack(ImGuiCol_Button, colorN, ImGuiCol_ButtonHovered, colorH,
                                        ImGuiCol_ButtonActive, colorP);
            ScopedFont boldFontScope(boldFont);
            if (ImGui::Button(label.c_str(), buttonSize)) {
                value = resetValue;
            }
        }
        ImGui::SameLine(0.0f, spacing);
        ImGui::SetNextItemWidth(inputItemWidth);

        UI::DragFloat(("##" + label).c_str(), &value, 0.1f, 0.0f, 0.0f, "%.2f");
    };

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
    drawControl("X", values.x, resetValues.x, ImVec4{0.8f, 0.1f, 0.15f, 1.0f},
                ImVec4{0.9f, 0.2f, 0.2f, 1.0f}, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});

    ImGui::SameLine(0.0f, spacing);
    drawControl("Y", values.y, resetValues.y, ImVec4{0.2f, 0.7f, 0.2f, 1.0f},
                ImVec4{0.3f, 0.8f, 0.3f, 1.0f}, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});

    ImGui::SameLine(0.0f, spacing);
    drawControl("Z", values.z, resetValues.z, ImVec4{0.1f, 0.25f, 0.8f, 1.0f},
                ImVec4{0.2f, 0.35f, 0.9f, 1.0f}, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});

    ImGui::EndChild();
}

// Widgets
static bool ComponentHeader(const char* label, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None) {
    std::string const& cc = "##dummy_id_" + std::string(label);
    bool open;
    {
        ScopedStyle style(ImGuiStyleVar_FrameRounding, 0.0f);
        open = ImGui::CollapsingHeader(cc.c_str(), flags | ImGuiTreeNodeFlags_SpanFullWidth);
        ImGui::SetItemAllowOverlap();
    }

    const ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[1];

    Ref<Texture> icon = EditorResource::AssetIcon;
    ScopedFont boldFontScope(boldFont);
    ImGui::SameLine();
    {
        ShiftCursorY(2.0f);
        ScopedColorStack style(ImGuiCol_Border, IM_COL32(0, 0, 0, 0), ImGuiCol_Button,
                               IM_COL32(0, 0, 0, 0));
        UI::ImageButton((cc + "AssetIcon").c_str(),
                        GlobalContext::GetDriverApi()->GetTextureID(icon), ImVec2(16.0f, 16.0f),
                        ImU32(IM_COL32(196, 196, 196, 255)), ImU32(IM_COL32(196, 196, 196, 255)),
                        ImU32(IM_COL32(196, 196, 196, 255)));

        ShiftCursorY(-2.0f);
    }
    ImGui::SameLine(0.0f, 20.0f);
    ImGui::TextUnformatted(label);

    icon = EditorResource::MenuDotsIcon;
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 30.0f, 0.0f);
    {
        ShiftCursorY(2.0f);
        ScopedColorStack style(ImGuiCol_Border, IM_COL32(0, 0, 0, 0), ImGuiCol_Button,
                               IM_COL32(0, 0, 0, 0));
        if (UI::ImageButton((cc + "MenuDotsIcon").c_str(),
                            GlobalContext::GetDriverApi()->GetTextureID(icon),
                            ImVec2(16.0f, 16.0f))) {
            // TODO: Open context menu
        }

        ShiftCursorY(-2.0f);
    }

    return open;
}

/*!
 *
 * @param id_label prevent id conflict when use ListHeader's Element 0/1/2...
 * @param type_label identifier for drag drop system, usually the type of the payload can be used
 * @param payload the payload in this drag drop bar & the payload will be changed if the drag drop
 * operation is successful
 * @param assetType the payload type of this drag drop bar
 * @param label_shift
 * @param text_shift
 * @return
 */
static bool DragDropBar(const char* id_label, const char* type_label, AssetHandle& payload,
                        AssetType assetType, float label_shift = 20.0f, float text_shift = 50.0f) {
    bool modified = false;
    UI::ScopedStyle style(ImGuiStyleVar_FrameRounding, 2.0f);
    ScopedID id(id_label);

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + label_shift);
    ImGui::TextUnformatted(type_label);

    ImGui::SameLine(0.0f, text_shift);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10.0f);
    std::string payload_name = AssetManager::IsAssetHandleValid(payload)
                                   ? AssetManager::GetMetadata(payload).FilePath.stem().string()
                                   : "None";
    // TODO: Replace the input text with text (disable maybe?) for better
    ImGui::InputText("##dragdrop", (char*)payload_name.c_str(), 256,
                     ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_NoMarkEdited);
    ImGui::SetItemAllowOverlap();
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* accept_payload =
                ImGui::AcceptDragDropPayload(Utils::AssetTypeToString(assetType))) {
            IM_ASSERT(accept_payload->DataSize == sizeof(AssetHandle));
            payload = *(const AssetHandle*)accept_payload->Data;
            modified = true;
        }
        ImGui::EndDragDropTarget();
    }
    DrawItemActivityOutline();

    ImGui::SameLine(0.0f, 0.0f);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 20.0f);

    {
        auto icon = EditorResource::ExchangeIcon;
        ShiftCursorY(2.0f);
        ScopedColorStack style(ImGuiCol_Border, IM_COL32(0, 0, 0, 0), ImGuiCol_Button,
                               IM_COL32(0, 0, 0, 0), ImGuiCol_ButtonHovered, IM_COL32(0, 0, 0, 0));
        if (UI::ImageButton(id_label, GlobalContext::GetDriverApi()->GetTextureID(icon),
                            ImVec2(16.0f, 16.0f))) {
            // TODO: Open component select menu
        }

        ShiftCursorY(-2.0f);
    }
    //            DrawItemActivityOutline();
    return modified;
}

/*!
 *
 * @param label
 * @param payloads
 * @return
 */
static bool ListHeader(const char* label, std::vector<AssetHandle>& payloads, AssetType assetType) {
    int value = payloads.size();
    std::string cc = "##dummy_id_" + std::string(label) + "_";
    bool open;
    bool modified = false;
    {
        UI::ScopedColorStack(ImGuiCol_Border, IM_COL32(0, 0, 0, 0), ImGuiCol_Header,
                             Colors::Theme::background);
        open = ImGui::CollapsingHeader(cc.c_str());
    }

    const ImGuiIO& io = ImGui::GetIO();
    ImFont* boldFont = io.Fonts->Fonts[1];

    {
        UI::ScopedFont font(boldFont);
        ImGui::SameLine();
        ImGui::TextUnformatted(label);
    }

    if (value <= 1) value = 1;
    if (open) {
        float spacing = 30.0f;  // child window left and right spacing
        float height = 30.0f;
        float tocal_height = height * value + 2.0f;
        float width = ImGui::GetWindowContentRegionWidth() - 20.0f - spacing;

        if (value > 0) {
            UI::ScopedColorStack colorStack(ImGuiCol_Border, IM_COL32(40, 40, 40, 255),
                                            ImGuiCol_ChildBg, IM_COL32(65, 65, 65, 255));
            UI::ScopedStyleStack styleStack(ImGuiStyleVar_ChildBorderSize, 1.0f,
                                            ImGuiStyleVar_ChildRounding, 2.0f);

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + spacing);
            ImGui::BeginChild(ImGui::GetID((std::string(label) + "child").c_str()),
                              ImVec2(width, tocal_height), true, ImGuiWindowFlags_NoScrollbar);
            for (int i = 0; i < value; i++) {
                float cury = ImGui::GetCursorPosY();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
                //                        ImGui::Button("A", {20.0f, 20.0f});
                {
                    ShiftCursorY(2.0f);
                    ScopedColorStack style(ImGuiCol_Border, IM_COL32(0, 0, 0, 0), ImGuiCol_Button,
                                           IM_COL32(0, 0, 0, 0));
                    if (UI::ImageButton((cc + "BurgerMenuIcon " + std::to_string(i)).c_str(),
                                        GlobalContext::GetDriverApi()->GetTextureID(
                                            EditorResource::BurgerMenuIcon),
                                        ImVec2(16.0f, 16.0f), ImU32(IM_COL32(196, 196, 196, 255)),
                                        ImU32(IM_COL32(196, 196, 196, 255)),
                                        ImU32(IM_COL32(196, 196, 196, 255)))) {
                        //                                ET_CORE_INFO("BurgerMenuIcon Pressed");
                    }

                    //                            ShiftCursorY(-2.0f);
                }
                ImGui::SameLine();
                ShiftCursorY(3.0f);  // Wired bug
                std::string elementlabel = "Element " + std::to_string(i);
                auto& tmp = payloads[i];
                modified =
                    modified || DragDropBar((cc + elementlabel).c_str(), elementlabel.c_str(),
                                            payloads[i], assetType, 5.0f, 10.0f);
                ImGui::SetCursorPosY(cury + height);
            }
            ImGui::EndChild();
        }

        ImGui::SetCursorPosX(width - 40.0f);
        UI::ShiftCursorY(-4.0f);
        auto icon = EditorResource::PlusIcon;
        if (UI::ImageButton((cc + "PlusIcon").c_str(),
                            GlobalContext::GetDriverApi()->GetTextureID(icon),
                            ImVec2(16.0f, 16.0f))) {
            // TODO:: Add global default material
            payloads.push_back(AssetHandle(9102962350420186113));
        }
        ImGui::SameLine(0.0f, 5.0f);
        icon = EditorResource::MinusIcon;
        if (UI::ImageButton((cc + "MinusIcon").c_str(),
                            GlobalContext::GetDriverApi()->GetTextureID(icon),
                            ImVec2(16.0f, 16.0f))) {
            payloads.pop_back();
        }
    }

    return modified;
}

};  // namespace UI

}  // namespace Ethereal
