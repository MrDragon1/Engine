#include "pch.h"
#include "GLInput.h"

#include "Base/Application.h"
#include <GLFW/glfw3.h>

namespace Ethereal {
Input* Input::sInstance = new GLInput();

bool GLInput::IsKeyPressedImpl(KeyCode keycode) {
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, static_cast<int32_t>(keycode));
    return state == GLFW_PRESS;
}
bool GLInput::IsMouseButtonPressedImpl(MouseCode button) {
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
    return state == GLFW_PRESS;
}
std::pair<float, float> GLInput::GetMousePositionImpl() {
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    return {(float)xpos, (float)ypos};
}
float GLInput::GetMouseXImpl() {
    auto [x, y] = GetMousePositionImpl();
    return x;
}
float GLInput::GetMouseYImpl() {
    auto [x, y] = GetMousePositionImpl();
    return y;
}

void GLInput::SetMousePosImpl(const Vector2& pos) {
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    glfwSetCursorPos(window, pos.x, pos.y);
}

void GLInput::SetMouseVisibleImpl() {
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
void GLInput::SetMouseInvisibleImpl() {
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
}  // namespace Ethereal