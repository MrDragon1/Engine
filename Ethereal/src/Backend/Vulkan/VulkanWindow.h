#pragma once

#include "Base/Window.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Ethereal {
class VulkanWindow : public Window {
   public:
    VulkanWindow(const WindowProps& props);
    virtual ~VulkanWindow();

    void OnUpdate() override;

    inline uint32_t GetWidth() const override { return mData.Width; }
    inline uint32_t GetHeight() const override { return mData.Height; }

    inline void SetEventCallback(const EventCallbackFn& callback) override {
        mData.EventCallback = callback;
    }

    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

    inline virtual void* GetNativeWindow() const override { return mWindow; }

    void GetRequiredExtensions(std::vector<const char*>& extensions) override;

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
