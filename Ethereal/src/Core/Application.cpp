#include "pch.h"
#include "Application.h"

#include "Core/Input.h"
#include "Core/Log.h"
#include "Event/ApplicationEvent.h"
#include "Utils/PlatformUtils.h"
#include "Core/GlobalContext.h"
namespace Ethereal
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const std::string& name) {
        ET_CORE_ASSERT(!s_Instance, "Application already exists!")
        s_Instance = this;
        m_Window = Window::Create(WindowProps(name));
        m_Window->SetEventCallback(ET_BIND_EVENT_FN(Application::OnEvent));

        RenderCommand::Init();
        GlobalContext::Reset();
        
        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
    }

    Application::~Application() {}

    void Application::PushLayer(Layer* layer) {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer) {
        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    void Application::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(ET_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(ET_BIND_EVENT_FN(Application::OnWindowResize));

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
            if (e.Handled) break;
            (*it)->OnEvent(e);
        }
    }

    void Application::Run() {
        while (m_Running) {
            float time = Time::GetTime();
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            if (!m_Minimized) {
                for (Layer* layer : m_LayerStack) layer->OnUpdate(timestep);
            }
            m_ImGuiLayer->Begin();
            for (Layer* layer : m_LayerStack) layer->OnImGuiRender();
            m_ImGuiLayer->End();
            m_Window->OnUpdate();
        }
    }

    void Application::Close() { m_Running = false; }

    bool Application::OnWindowClose(WindowCloseEvent& e) {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e) {
        if (e.GetWidth() == 0 || e.GetHeight() == 0) {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;
        RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
        return false;
    }

}  // namespace Ethereal
