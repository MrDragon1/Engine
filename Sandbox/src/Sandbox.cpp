#include "Engine.hpp"

class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
	
	}

	void OnEvent(Engine::Event& event) override
	{
		
	}

};
class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
        PushLayer(new ExampleLayer());
        PushOverlay(new Engine::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}
