#pragma once

#include "Base/Layer/Layer.h"

#include "Base/Event/ApplicationEvent.h"
#include "Base/Event/KeyEvent.h"
#include "Base/Event/MouseEvent.h"

namespace Ethereal
{
    class ImGuiLayer : public Layer {
      public:
        ImGuiLayer();
        ~ImGuiLayer();

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImGuiRender() override;
        virtual void OnEvent(Event& e) override;

        void BlockEvents(bool block) { mBlockEvents = block; }
        void Begin();
        void End();

      private:
        void SetDarkThemeColors();

      private:
        float mTime = 0.0f;
        bool mBlockEvents = true;
    };
}  // namespace Ethereal