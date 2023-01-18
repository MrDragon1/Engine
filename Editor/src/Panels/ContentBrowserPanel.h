#pragma once
#include "Core/Editor/EditorPanel.h"
#include "Core/Renderer/Texture.h"

#include <filesystem>

namespace Ethereal
{
    class ContentBrowserPanel : public EditorPanel{
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