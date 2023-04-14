#pragma once

#include "Base/Event/ApplicationEvent.h"
#include "Base/ImGui/ImGuiLayer.h"
#include "Base/Layer/Layer.h"
#include "Base/Layer/LayerStack.h"
#include "Base/Window.h"
#include "Utils/EngineMarco.h"

namespace Ethereal {
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

    /// Creates & Dispatches an event either immediately, or adds it to an event queue which will be proccessed at the end of each frame
    template <typename TEvent, bool DispatchImmediately = true, typename... TEventArgs>
    void DispatchEvent(TEventArgs&&... args) {
        static_assert(std::is_assignable_v<Event, TEvent>);

        std::shared_ptr<TEvent> event = std::make_shared<TEvent>(std::forward<TEventArgs>(args)...);
        if constexpr (DispatchImmediately) {
            OnEvent(*event);
        } else {
            ET_CORE_WARN("Not support event queue yet!");
        }
    }

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
