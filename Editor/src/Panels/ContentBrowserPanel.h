#pragma once
#include <filesystem>

#include "Core/Editor/EditorPanel.h"

namespace Ethereal {
class ContentBrowserPanel : public EditorPanel {
   public:
    ContentBrowserPanel();

    void OnImGuiRender(bool& isOpen) override;
    void OnEvent(Event& event) override;

    void SetSceneContext(const Ref<Scene>& context) override { mSceneContext = context; }

   private:
    std::filesystem::path mCurrentDirectory;
    std::filesystem::path mSelectedDirectory;
    Ref<Scene> mSceneContext;
};
}  // namespace Ethereal