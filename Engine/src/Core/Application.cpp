#include "Application.hpp"
#include "Event/ApplicationEvent.hpp"
#include "Core/Log.hpp"
namespace Engine
{
	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
        WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication))
		{
			ENGINE_TRACE(e);
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			ENGINE_TRACE(e);
		}
		while (true);
	}
}
