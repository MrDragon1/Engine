#include "Application.h"

#include "Base/Event/ApplicationEvent.h"
#include "Base/GlobalContext.h"
#include "Base/Log.h"
#include "Base/Meta/ReflectionRegister.h"
#include "Base/Meta/Serializer.h"
#include "Core/Project/Project.h"
#include "Utils/PlatformUtils.h"

namespace Ethereal {
Application* Application::sInstance = nullptr;

Application::Application(const std::string& name) {
    ET_CORE_ASSERT(!sInstance, "Application already exists!")
    sInstance = this;
    mWindow = Window::Create(WindowProps(name));
    mWindow->SetEventCallback(ET_BIND_EVENT_FN(Application::OnEvent));
    Reflection::TypeMetaRegister::Register();

    GlobalContext::Init();

    GlobalContext::Reset();

    mImGuiLayer = new ImGuiLayer();
    PushOverlay(mImGuiLayer);
}

Application::~Application() {
    for (auto& layer : mLayerStack) {
        if (layer) layer->OnDetach();
    }

    Project::SetActive(nullptr);
}

void Application::PushLayer(Layer* layer) {
    mLayerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer* layer) {
    mLayerStack.PushOverlay(layer);
    layer->OnAttach();
}

void Application::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(ET_BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(ET_BIND_EVENT_FN(Application::OnWindowResize));

    for (auto it = mLayerStack.rbegin(); it != mLayerStack.rend(); ++it) {
        if (e.Handled) break;
        (*it)->OnEvent(e);
    }
}

void Application::Run() {
    while (mRunning) {
        float time = Time::GetTime();
        TimeStamp timestep = time - mLastFrameTime;
        mLastFrameTime = time;

        if (!mMinimized) {
            for (Layer* layer : mLayerStack) layer->OnUpdate(timestep);
        }
        mImGuiLayer->Begin();
        for (Layer* layer : mLayerStack) layer->OnImGuiRender();
        mImGuiLayer->End();
        mWindow->OnUpdate();
    }
}

void Application::Close() { mRunning = false; }

bool Application::OnWindowClose(WindowCloseEvent& e) {
    mRunning = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e) {
    if (e.GetWidth() == 0 || e.GetHeight() == 0) {
        mMinimized = true;
        return false;
    }

    mMinimized = false;
    GlobalContext::GetDriverApi()->SetViewport(0, 0, e.GetWidth(), e.GetHeight());
    return false;
}

}  // namespace Ethereal
