#include "VulkanWindow.h"
#include "pch.h"

#include "Base/Event/ApplicationEvent.h"
#include "Base/Event/KeyEvent.h"
#include "Base/Event/MouseEvent.h"
namespace Ethereal {

VulkanWindow::VulkanWindow(const WindowProps& props) { Init(props); }

VulkanWindow::~VulkanWindow() { Shutdown(); }

void VulkanWindow::OnUpdate() { glfwPollEvents(); }

void VulkanWindow::SetVSync(bool enabled) { mData.VSync = enabled; }

bool VulkanWindow::IsVSync() const { return mData.VSync; }

void VulkanWindow::GetRequiredExtensions(std::vector<const char*>& extensions) {
    extensions.clear();
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        extensions.push_back(glfwExtensions[i]);
    }
}

void VulkanWindow::Init(const WindowProps& props) {
    mData.Title = props.Title;
    mData.Width = props.Width;
    mData.Height = props.Height;

    ET_CORE_INFO("Creating window {0} ({1}, {2})", mData.Title, mData.Width, mData.Height);
    if (!sGLFWInitialized) {
        int success = glfwInit();
        ET_CORE_ASSERT(success, "Could not initialize GLFW!");
        glfwSetErrorCallback(GLFWErrorCallback);
        sGLFWInitialized = true;
    }
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mWindow = glfwCreateWindow(mData.Width, mData.Height, mData.Title.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(mWindow, &mData);
    SetVSync(true);

    // Set GLFW callbacks
    glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int width, int height) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.Width = width;
        data.Height = height;

        WindowResizeEvent event(width, height);
        data.EventCallback(event);
    });

    glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        WindowCloseEvent event;
        data.EventCallback(event);
    });

    glfwSetKeyCallback(mWindow,
                       [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                           WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                           switch (action) {
                               case GLFW_PRESS: {
                                   KeyPressedEvent event(key, 0);
                                   data.EventCallback(event);
                                   break;
                               }
                               case GLFW_RELEASE: {
                                   KeyReleasedEvent event(key);
                                   data.EventCallback(event);
                                   break;
                               }
                               case GLFW_REPEAT: {
                                   KeyPressedEvent event(key, true);
                                   data.EventCallback(event);
                                   break;
                               }
                           }
                       });

    glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned int keycode) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        KeyTypedEvent event(keycode);
        data.EventCallback(event);
    });

    glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        switch (action) {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event(button);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event(button);
                data.EventCallback(event);
                break;
            }
        }
    });

    glfwSetScrollCallback(mWindow, [](GLFWwindow* window, double xOffset, double yOffset) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        MouseScrolledEvent event((float)xOffset, (float)yOffset);
        data.EventCallback(event);
    });

    glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xPos, double yPos) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        MouseMovedEvent event((float)xPos, (float)yPos);
        data.EventCallback(event);
    });
}

void VulkanWindow::Shutdown() { glfwTerminate(); }

}  // namespace Ethereal
