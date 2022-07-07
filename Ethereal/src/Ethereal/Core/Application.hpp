#pragma once

#include "Ethereal/Event/ApplicationEvent.hpp"
#include "Ethereal/ImGui/ImGuiLayer.hpp"
#include "Ethereal/Core/Layer.hpp"
#include "Ethereal/Core/LayerStack.hpp"
#include "Ethereal/Core/Window.hpp"
#include "Ethereal/Event/Event.hpp"
#include "Utils.hpp"

namespace Ethereal
{
    class Application {
      public:
        Application(const std::string& name = "Engine");
        ~Application();

        void Run();
        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);
        Window& GetWindow() { return *m_Window; }
        ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

        static Application& Get() { return *s_Instance; }
        void Close();

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
}  // namespace Ethereal