#pragma once

#include "Core/Layer.h"

#include "Event/ApplicationEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"

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

        void BlockEvents(bool block) { m_BlockEvents = block; }
        void Begin();
        void End();

      private:
        void SetDarkThemeColors();

      private:
        float m_Time = 0.0f;
        bool m_BlockEvents = true;
    };
}  // namespace Ethereal