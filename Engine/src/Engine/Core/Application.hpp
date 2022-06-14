#pragma once

#include "Engine/Event/ApplicationEvent.hpp"
#include "Engine/ImGui/ImGuiLayer.hpp"
#include "Engine/Core/Layer.hpp"
#include "Engine/Core/LayerStack.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Event/Event.hpp"
#include "Utils.hpp"

namespace Engine
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
}  // namespace Engine
