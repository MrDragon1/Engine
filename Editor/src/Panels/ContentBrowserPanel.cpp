#include "ContentBrowserPanel.h"

#include <Base/ImGui/UI.h>
#include <Core/Editor/EditorResource.h>

#include "Core/Asset/AssetManager.h"
#include "Base/GlobalContext.h"
#include "imgui.h"
#include "pch.h"

namespace Ethereal {
namespace Utils {
static bool IsImageFormat(std::string filePath) {
    std::string extension = filePath.substr(filePath.find_last_of(".") + 1);
    if (extension == "png" || extension == "jpg" || extension == "bmp" || extension == "tga") {
        return true;
    }
    return false;
}
}  // namespace Utils

ContentBrowserPanel::ContentBrowserPanel() : mCurrentDirectory(Project::GetAssetDirectory()) {}

void ContentBrowserPanel::OnEvent(Event& event) {}

void ContentBrowserPanel::OnImGuiRender(bool& isOpen) {
    auto api = GlobalContext::GetDriverApi();
    ImGui::Begin("Content Browser", &isOpen);
    if (mCurrentDirectory != Project::GetAssetDirectory()) {
        if (ImGui::Button("<-")) {
            mSelectedDirectory = mCurrentDirectory;
            mCurrentDirectory = mCurrentDirectory.parent_path();
        }
    }
    static float padding = 4.0f;
    static float thumbnailSize = 80.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1) columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    for (auto& directoryEntry : std::filesystem::directory_iterator(mCurrentDirectory)) {
        const auto& path = directoryEntry.path();
        std::string filenameString = path.filename().string();
        bool IsSelect = mSelectedDirectory == directoryEntry.path();
        ImGui::PushID(filenameString.c_str());

        Ref<Texture> icon =
            directoryEntry.is_directory()
                ? EditorResource::FolderIcon
                : AssetManager::GetAssetIconFromExtension(path.extension().string());
        {
            UI::ScopedColorStack style(ImGuiCol_Border, IM_COL32(0, 0, 0, 0), ImGuiCol_Button,
                                       IM_COL32(0, 0, 0, 0), ImGuiCol_ButtonHovered,
                                       IM_COL32(0, 0, 0, 0));
            UI::ShiftCursorX(-5.0f);
            bool press = false;
            if (Utils::IsImageFormat(path.string())) {
                Ref<Texture> img = AssetManager::GetAsset<Texture>(path.string()).As<Texture>();
                press = ImGui::ImageButton(api->GetTextueID(img), {thumbnailSize, thumbnailSize},
                                           {0, 1}, {1, 0});
            } else
                press = ImGui::ImageButton(api->GetTextueID(icon), {thumbnailSize, thumbnailSize},
                                           {0, 1}, {1, 0});

            if (press) {
                mSelectedDirectory = directoryEntry.path();
                IsSelect = true;
            }
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            AssetHandle handle = AssetManager::GetAssetHandleFromFilePath(path);
            ImGui::SetDragDropPayload(AssetManager::GetDisplayTypeName(handle).c_str(), &handle,
                                      sizeof(AssetHandle));
            ImGui::EndDragDropSource();
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            if (directoryEntry.is_directory()) mCurrentDirectory /= path.filename();
        }
        const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
        const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

        std::string filename = path.stem().string();
        if (IsSelect) {
            UI::ScopedColorStack color(ImGuiCol_Border, IM_COL32(0, 0, 0, 0), ImGuiCol_Button,
                                       IM_COL32(44, 93, 135, 255), ImGuiCol_ButtonHovered,
                                       IM_COL32(44, 93, 135, 255), ImGuiCol_Text,
                                       IM_COL32(210, 221, 230, 255));
            UI::ScopedStyle stylepadding(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::Button(filename.c_str(), {thumbnailSize, 0});
        } else {
            UI::ScopedColorStack color(ImGuiCol_Border, IM_COL32(0, 0, 0, 0), ImGuiCol_Button,
                                       IM_COL32(0, 0, 0, 0), ImGuiCol_ButtonHovered,
                                       IM_COL32(0, 0, 0, 0));
            UI::ScopedStyle stylepadding(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            //                ImGui::TextWrapped(filenameString.c_str());
            ImGui::Button(filename.c_str(), {thumbnailSize, 0});
        }
        ImGui::NextColumn();

        ImGui::PopID();
    }
    ImGui::Columns(1);

    //                ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);

    // TODO: status bar
    ImGui::End();
}
}  // namespace Ethereal