#pragma once
#include <filesystem>

#include "Core/Editor/EditorPanel.h"

namespace Ethereal {
class ContentBrowserPanel : public EditorPanel {
   public:
    ContentBrowserPanel();

    void OnImGuiRender(bool& isOpen) override;
    void OnEvent(Event& event) override;

    void SetSceneContext(const Ref<Scene>& context) override { m_SceneContext = context; }

   private:
    std::filesystem::path m_CurrentDirectory;
    std::filesystem::path m_SelectedDirectory;
    Ref<Scene> m_SceneContext;
};
}  // namespace Ethereal