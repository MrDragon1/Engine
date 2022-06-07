#pragma once

#include "Utils.hpp"
#include "Event/Event.hpp"
#include "Core/Event/ApplicationEvent.hpp"
#include "Core/Window.hpp"

namespace Engine
{
	class Application
	{
	public:
		Application();
		~Application();

		void Run();
        void OnEvent(Event& e);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

