#pragma once

#include "Base/Input.h"

namespace Ethereal {
class GLInput : public Input {
   protected:
    virtual bool IsKeyPressedImpl(KeyCode keycode) override;
    virtual bool IsMouseButtonPressedImpl(MouseCode button) override;
    virtual std::pair<float, float> GetMousePositionImpl() override;
    virtual float GetMouseXImpl() override;
    virtual float GetMouseYImpl() override;

    virtual void SetMousePosImpl(const Vector2& pos) override;

    virtual void SetMouseVisibleImpl() override;
    virtual void SetMouseInvisibleImpl() override;
};
}  // namespace Ethereal
