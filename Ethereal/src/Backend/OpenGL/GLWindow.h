#pragma once

#include "Base/Window.h"
#include "GLFW/glfw3.h"
namespace Ethereal {
class GLWindow : public Window {
   public:
    GLWindow(const WindowProps& props);
    virtual ~GLWindow();

    void OnUpdate() override;

    inline uint32_t GetWidth() const override { return mData.Width; }
    inline uint32_t GetHeight() const override { return mData.Height; }

    inline void SetEventCallback(const EventCallbackFn& callback) override { mData.EventCallback = callback; }

    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

    inline virtual void* GetNativeWindow() const override { return mWindow; }

   private:
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();

   private:
    GLFWwindow* mWindow;

    struct WindowData {
        std::string Title;
        uint32_t Width, Height;
        bool VSync;
        EventCallbackFn EventCallback;
    };

    WindowData mData;
};
}  // namespace Ethereal