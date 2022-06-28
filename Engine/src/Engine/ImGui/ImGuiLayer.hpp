#pragma once

#include "Engine/Core/Layer.hpp"

#include "Engine/Event/ApplicationEvent.hpp"
#include "Engine/Event/KeyEvent.hpp"
#include "Engine/Event/MouseEvent.hpp"

namespace Engine
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
}  // namespace Engine