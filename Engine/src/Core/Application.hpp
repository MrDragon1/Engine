#pragma once

#include "Utils.hpp"
#include "Event/Event.hpp"

namespace Engine
{
	class Application
	{
	public:
		Application();
		~Application();

		void Run();
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

