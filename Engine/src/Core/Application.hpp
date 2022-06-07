#pragma once

#include "Utils.hpp"
#include "Event/Event.hpp"
#include "Core/Event/ApplicationEvent.hpp"
#include "Core/Window.hpp"
#include "Core/Layer.hpp"
#include "Core/LayerStack.hpp"
namespace Engine
{
	class Application
	{
	public:
		Application();
		~Application();

		void Run();
        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
        LayerStack m_LayerStack;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

