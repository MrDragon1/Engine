#pragma once

#include "Core/Layer.hpp"

#include "Core/Event/ApplicationEvent.hpp"
#include "Core/Event/KeyEvent.hpp"
#include "Core/Event/MouseEvent.hpp"

namespace Engine
{
    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImGuiRender() override;

        void Begin();
        void End();
    private:
        float m_Time = 0.0f;
    };
}