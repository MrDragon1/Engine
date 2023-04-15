#pragma once
#include "Utils/EngineMarco.h"
#include "Base/KeyCodes.h"
#include "Base/MouseButtonCodes.h"

namespace Ethereal {
class Input {
   public:
    inline static bool IsKeyPressed(KeyCode key) { return sInstance->IsKeyPressedImpl(key); }
    inline static bool IsMouseButtonPressed(MouseCode button) { return sInstance->IsMouseButtonPressedImpl(button); }
    inline static std::pair<float, float> GetMousePosition() { return sInstance->GetMousePositionImpl(); }
    inline static float GetMouseX() { return sInstance->GetMouseXImpl(); }
    inline static float GetMouseY() { return sInstance->GetMouseYImpl(); }

    inline static void SetMousePos(const Vector2& pos) { sInstance->SetMousePosImpl(pos); }
    inline static void SetMouseVisible() { sInstance->SetMouseVisibleImpl(); }
    inline static void SetMouseInvisible() { sInstance->SetMouseInvisibleImpl(); }

   protected:
    virtual bool IsKeyPressedImpl(KeyCode keycode) = 0;
    virtual bool IsMouseButtonPressedImpl(MouseCode button) = 0;
    virtual std::pair<float, float> GetMousePositionImpl() = 0;
    virtual float GetMouseXImpl() = 0;
    virtual float GetMouseYImpl() = 0;
    virtual void SetMousePosImpl(const Vector2& pos) = 0;

    virtual void SetMouseVisibleImpl() = 0;
    virtual void SetMouseInvisibleImpl() = 0;

   private:
    static Input* sInstance;
};
}  // namespace Ethereal