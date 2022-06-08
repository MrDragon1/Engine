#pragma once

#include "Core/Event/ApplicationEvent.hpp"
#include "Core/ImGui/ImGuiLayer.hpp"
#include "Core/Layer.hpp"
#include "Core/LayerStack.hpp"
#include "Core/Window.hpp"
#include "Event/Event.hpp"
#include "Utils.hpp"
#include "Core/Renderer/Shader.hpp"
#include "Core/Renderer/Buffer.hpp"
#include "Core/Renderer/VertexArray.hpp"
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
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
        ImGuiLayer* m_ImGuiLayer;
        LayerStack m_LayerStack;

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::shared_ptr<Shader> m_BlueShader;
		std::shared_ptr<VertexArray> m_SquareVA;

      private:
        static Application* s_Instance;
    };

    // To be defined in CLIENT
    Application* CreateApplication();
}  // namespace Engine
