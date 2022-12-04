#pragma once

#include "Base/ImGui/UI.h"

#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui
{
    bool TreeNodeWithIcon(Ethereal::Ref<Ethereal::Texture2D> icon, ImGuiID id, ImGuiTreeNodeFlags flags, const char* label, const char* label_end, ImColor iconTint = IM_COL32_WHITE);
    bool TreeNodeWithIcon(Ethereal::Ref<Ethereal::Texture2D> icon, const void* ptr_id, ImGuiTreeNodeFlags flags, ImColor iconTint, const char* fmt, ...);
    bool TreeNodeWithIcon(Ethereal::Ref<Ethereal::Texture2D> icon, const char* label, ImGuiTreeNodeFlags flags, ImColor iconTint = IM_COL32_WHITE);
}  // namespace Ethereal
