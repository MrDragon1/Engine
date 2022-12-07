#pragma once
#include <Core/Asset/AssetManager.h>
#include "pch.h"
#include "ImGuiUtils.h"
#include "Core/Renderer/Texture.h"
#include "Utils/StringUtils.h"

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

        struct PropertyAssetReferenceSettings
        {
            bool AdvanceToNextColumn = true;
            bool NoItemSpacing = false; // After label
            float WidthOffset = 0.0f;
            bool AllowMemoryOnlyAssets = false;
            ImVec4 ButtonLabelColor = ImGui::ColorConvertU32ToFloat4(Colors::Theme::text);
            bool ShowFullFilePath = false;
        };

        void Image(const Ref<Texture2D>& texture, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));

        static bool TreeNodeWithIcon(const std::string& label, const Ref<Texture2D>& icon, const ImVec2& size, bool openByDefault = true)
        {
            ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Framed
                                               | ImGuiTreeNodeFlags_SpanAvailWidth
                                               | ImGuiTreeNodeFlags_AllowItemOverlap
                                               | ImGuiTreeNodeFlags_FramePadding;

            if (openByDefault)
                treeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

            bool open = false;
            const float framePaddingX = 6.0f;
            const float framePaddingY = 6.0f; // affects height of the header

            UI::ScopedStyle headerRounding(ImGuiStyleVar_FrameRounding, 0.0f);
            UI::ScopedStyle headerPaddingAndHeight(ImGuiStyleVar_FramePadding, ImVec2{ framePaddingX, framePaddingY });

            ImGui::PushID(label.c_str());
            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
            open = ImGui::TreeNodeEx("##dummy_id", treeNodeFlags, "");

            float lineHeight = ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y;
            ImGui::SameLine();
            UI::ShiftCursorY(size.y / 4.0f);
            UI::Image(icon, size);
            UI::ShiftCursorY(-(size.y / 4.0f));
            ImGui::SameLine();
            ImGui::TextUnformatted(Utils::ToUpper(label).c_str());

            ImGui::PopID();

            return open;
        }

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

        static const char* GenerateLabelID(std::string_view label)
        {
            *fmt::format_to_n(s_LabelIDBuffer, std::size(s_LabelIDBuffer), "{}##{}", label, s_Counter++).out = 0;
            return s_LabelIDBuffer;
        }

        static void BeginDisabled(bool disabled = true)
        {
            if (disabled)
                ImGui::BeginDisabled(true);
        }

        static void EndDisabled()
        {
            // NOTE(Peter): Cheeky hack to prevent ImGui from asserting (required due to the nature of UI::BeginDisabled)
            if (GImGui->DisabledStackSize > 0)
                ImGui::EndDisabled();
        }

        static const char* GenerateID()
        {
            itoa(s_Counter++, s_IDBuffer + 2, 16);
            return s_IDBuffer;
        }

        static void HelpMarker(const char* desc)
        {
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted(desc);
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        }

        static void Property(const char* label, const std::string& value)
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);
            BeginDisabled();
            ImGui::InputText(GenerateID(), (char*)value.c_str(), value.size(), ImGuiInputTextFlags_ReadOnly);
            EndDisabled();
            
            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();
        }

        static void Property(const char* label, const char* value)
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);
            BeginDisabled();
            ImGui::InputText(GenerateID(), (char*)value, 256, ImGuiInputTextFlags_ReadOnly);
            EndDisabled();

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();
        }

        static bool Property(const char* label, char* value, size_t length)
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            bool modified = ImGui::InputText(GenerateID(), value, length);

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        static bool Property(const char* label, bool& value, const char* helpText = "")
        {
            bool modified = false;

            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            if (std::strlen(helpText) != 0)
            {
                ImGui::SameLine();
                HelpMarker(helpText);
            }
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            modified = ImGui::Checkbox(GenerateID(), &value);

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        static bool Property(const char* label, int8_t& value, int8_t min = 0, int8_t max = 0)
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            bool modified = UI::DragInt8(GenerateID(), &value, 1.0f, min, max);

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        static bool Property(const char* label, int16_t& value, int16_t min = 0, int16_t max = 0)
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            bool modified = UI::DragInt16(GenerateID(), &value, 1.0f, min, max);

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        static bool Property(const char* label, int32_t& value, int32_t min = 0, int32_t max = 0)
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            bool modified = UI::DragInt32(GenerateID(), &value, 1.0f, min, max);

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        static bool Property(const char* label, int64_t& value, int64_t min = 0, int64_t max = 0)
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            bool modified = UI::DragInt64(GenerateID(), &value, 1.0f, min, max);

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        static bool Property(const char* label, uint8_t& value, uint8_t minValue = 0, uint8_t maxValue = 0)
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            bool modified = UI::DragUInt8(GenerateID(), &value, 1.0f, minValue, maxValue);

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        static bool Property(const char* label, uint16_t& value, uint16_t minValue = 0, uint16_t maxValue = 0)
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            bool modified = UI::DragUInt16(GenerateID(), &value, 1.0f, minValue, maxValue);

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        static bool Property(const char* label, uint32_t& value, uint32_t minValue = 0, uint32_t maxValue = 0)
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            bool modified = UI::DragUInt32(GenerateID(), &value, 1.0f, minValue, maxValue);

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        static bool Property(const char* label, uint64_t& value, uint64_t minValue = 0, uint64_t maxValue = 0)
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            bool modified = UI::DragUInt64(GenerateID(), &value, 1.0f, minValue, maxValue);

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        static bool Property(const char* label, float& value, float delta = 0.1f, float min = 0.0f, float max = 0.0f, const char* helpText = "")
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            if (std::strlen(helpText) != 0)
            {
                ImGui::SameLine();
                HelpMarker(helpText);
            }
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            bool modified = UI::DragFloat(GenerateID(), &value, delta, min, max);

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        static bool Property(const char* label, double& value, float delta = 0.1f, double min = 0.0, double max = 0.0)
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            bool modified = UI::DragDouble(GenerateID(), &value, delta, min, max);

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        static bool Property(const char* label, glm::vec2& value, float delta = 0.1f, float min = 0.0f, float max = 0.0f)
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            bool modified = ImGui::DragFloat2(GenerateID(), glm::value_ptr(value), delta, min, max);

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        static bool Property(const char* label, glm::vec3& value, float delta = 0.1f, float min = 0.0f, float max = 0.0f, const char* helpText = "")
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);

            if (std::strlen(helpText) != 0)
            {
                ImGui::SameLine();
                HelpMarker(helpText);
            }

            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            bool modified = ImGui::DragFloat3(GenerateID(), glm::value_ptr(value), delta, min, max);

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        static bool Property(const char* label, glm::vec4& value, float delta = 0.1f, float min = 0.0f, float max = 0.0f)
        {
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            bool modified = ImGui::DragFloat4(GenerateID(), glm::value_ptr(value), delta, min, max);

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        static bool PropertyDropdown(const char* label, const char** options, int32_t optionCount, int32_t* selected)
        {
            const char* current = options[*selected];
            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);

            bool modified = false;

            const std::string id = "##" + std::string(label);
            if (ImGui::BeginCombo(id.c_str(), current))
            {
                for (int i = 0; i < optionCount; i++)
                {
                    const bool is_selected = (current == options[i]);
                    if (ImGui::Selectable(options[i], is_selected))
                    {
                        current = options[i];
                        *selected = i;
                        modified = true;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            ImGui::NextColumn();
            Draw::Underline();

            return modified;
        }

        template<typename T, typename Fn>
        static bool PropertyAssetReferenceTarget(const char* label, const char* assetName, AssetHandle& outHandle, Fn&& targetFunc, const PropertyAssetReferenceSettings& settings = PropertyAssetReferenceSettings())
        {
            bool modified = false;

            ShiftCursor(10.0f, 9.0f);
            ImGui::Text(label);
            ImGui::NextColumn();
            ShiftCursorY(4.0f);
            ImGui::PushItemWidth(-1);
            if (settings.NoItemSpacing)
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

            ImVec2 originalButtonTextAlign = ImGui::GetStyle().ButtonTextAlign;
            ImGui::GetStyle().ButtonTextAlign = { 0.0f, 0.5f };
            float width = ImGui::GetContentRegionAvail().x - settings.WidthOffset;
            UI::PushID();

            float itemHeight = 28.0f;

            std::string buttonText = "Null";

            if (AssetManager::IsAssetHandleValid(outHandle))
            {
                auto source = AssetManager::GetAsset<T>(outHandle);
                if (source && !source->IsFlagSet(AssetFlag::Missing))
                {
                    if (assetName)
                    {
                        buttonText = assetName;
                    }
                    else
                    {
                        buttonText = AssetManager::GetMetadata(outHandle).FilePath.stem().string();
                        assetName = buttonText.c_str();
                    }
                }
                else
                {
                    buttonText = "Missing";
                }
            }

            if ((GImGui->CurrentItemFlags & ImGuiItemFlags_MixedValue) != 0)
                buttonText = "---";

            // PropertyAssetReferenceTarget could be called multiple times in same "context"
            // and so we need a unique id for the asset search popup each time.
            // notes
            // - don't use GenerateID(), that's inviting id clashes, which would be super confusing.
            // - don't store return from GenerateLabelId in a const char* here. Because its pointing to an internal
            //   buffer which may get overwritten by the time you want to use it later on.
            std::string assetSearchPopupID = GenerateLabelID("ARTSP");

            if (ImGui::Button(GenerateLabelID(buttonText), { width, itemHeight }))
            {
                ImGui::OpenPopup(assetSearchPopupID.c_str());
            }
            ImGui::GetStyle().ButtonTextAlign = originalButtonTextAlign;

            bool clear = false;
//            if (Widgets::AssetSearchPopup(assetSearchPopupID.c_str(), T::GetStaticType(), outHandle, &clear))
//            {
//                if (clear)
//                    outHandle = 0;
//
//                targetFunc(AssetManager::GetAsset<T>(outHandle));
//                modified = true;
//            }

            UI::PopID();

            if (ImGui::BeginDragDropTarget())
            {
                auto data = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");

                if (data)
                {
                    AssetHandle assetHandle = *(AssetHandle*)data->Data;
                    // s_PropertyAssetReferenceAssetHandle = assetHandle;
                    Ref<Asset> asset = AssetManager::GetAsset<Asset>(assetHandle);
                    if (asset && asset->GetAssetType() == T::GetStaticType())
                    {
                        targetFunc(asset.As<T>());
                        modified = true;
                    }
                }
            }

            if (!IsItemDisabled())
                DrawItemActivityOutline(2.0f, true, Colors::Theme::accent);

            ImGui::PopItemWidth();
            if (settings.AdvanceToNextColumn)
            {
                ImGui::NextColumn();
                Draw::Underline();
            }
            if (settings.NoItemSpacing)
                ImGui::PopStyleVar();
            return modified;
        }

    };  // namespace UI

}  // namespace Ethereal
