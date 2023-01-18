#include <Core/Editor/EditorResource.h>
#include <Base/ImGui/UI.h>
#include "pch.h"
#include "ContentBrowserPanel.h"
#include "Core/Asset/AssetManager.h"

#include "imgui.h"

namespace Ethereal
{
    namespace Utils
    {
        static bool IsImageFormat(std::string filePath) {
            std::string extension = filePath.substr(filePath.find_last_of(".") + 1);
            if (extension == "png" || extension == "jpg" || extension == "bmp" || extension == "tga") {
                return true;
            }
            return false;
        }
    }  // namespace Utils

    ContentBrowserPanel::ContentBrowserPanel() : m_CurrentDirectory(Project::GetAssetDirectory()) {

    }

    void ContentBrowserPanel::OnEvent(Event& event){

    }

    void ContentBrowserPanel::OnImGuiRender(bool& isOpen) {
        ImGui::Begin("Content Browser", &isOpen);
        if (m_CurrentDirectory != Project::GetAssetDirectory()) {
            if (ImGui::Button("<-")) {
                m_CurrentDirectory = m_CurrentDirectory.parent_path();
            }
        }

        static float padding = 32.0f;
        static float thumbnailSize = 80.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1) columnCount = 1;

        ImGui::Columns(columnCount, 0, false);

        for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
            const auto& path = directoryEntry.path();
            std::string filenameString = path.filename().string();

            ImGui::PushID(filenameString.c_str());

            Ref<Texture2D> icon = directoryEntry.is_directory() ? EditorResource::FolderIcon : EditorResource::FileIcon;
            {
                UI::ScopedColorStack style(ImGuiCol_Border, IM_COL32(0, 0, 0, 0),ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
                if (Utils::IsImageFormat(path.string())) {
                    Ref<Texture2D> img = AssetManager::GetAsset<Texture>(path.string()).As<Texture2D>();
                    ImGui::ImageButton((ImTextureID)img->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});
                } else
                    ImGui::ImageButton((ImTextureID)icon->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});

            }

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                AssetHandle handle = AssetManager::GetAssetHandleFromFilePath(path);
                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &handle, sizeof(AssetHandle));
                ImGui::EndDragDropSource();
            }


            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                if (directoryEntry.is_directory()) m_CurrentDirectory /= path.filename();
            }
            ImGui::TextWrapped(filenameString.c_str());
            ImGui::NextColumn();

            ImGui::PopID();
        }
        ImGui::Columns(1);

        //        ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
        //        ImGui::SliderFloat("Padding", &padding, 0, 32);

        // TODO: status bar
        ImGui::End();
    }
}  // namespace Ethereal