#pragma once

#include "Core/Event/ApplicationEvent.hpp"
#include "Core/ImGui/ImGuiLayer.hpp"
#include "Core/Layer.hpp"
#include "Core/LayerStack.hpp"
#include "Core/Window.hpp"
#include "Event/Event.hpp"
#include "Utils.hpp"

namespace Engine
{
    class Application {
      public:
        Application();
        ~Application();

        void Run();
        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);
        inline Window& GetWindow() { return *m_Window; }
        inline static Application& Get() { return *s_Instance; }

      private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

      private:
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
        bool m_Minimized = false;
        ImGuiLayer* m_ImGuiLayer;
        LayerStack m_LayerStack;
        float m_LastFrameTime = 0.0f;

      private:
        static Application* s_Instance;
    };

    // To be defined in CLIENT
    Application* CreateApplication();
}  // namespace Engine
