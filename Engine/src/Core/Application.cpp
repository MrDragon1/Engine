#include "Application.hpp"
#include "Core/Log.hpp"
#include "Event/ApplicationEvent.hpp"
#include "pch.hpp"

#include "GLFW/glfw3.h"

namespace Engine
{
	Application::Application()
	{
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(ENGINE_BIND_EVENT_FN(Application::OnEvent));
	}

	Application::~Application()
	{
	}

    void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(ENGINE_BIND_EVENT_FN(Application::OnWindowClose));

		ENGINE_CORE_TRACE("{0}", e);
	}

	void Application::Run()
	{
		while(m_Running)
        {
            glClearColor(1, 0, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            m_Window->OnUpdate();
        }
	}

    bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

}
