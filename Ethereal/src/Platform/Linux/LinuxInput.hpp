#pragma once

#include "Ethereal/Core/Input.hpp"

namespace Ethereal
{
    class WindowsInput : public Input {
      protected:
        virtual bool IsKeyPressedImpl(KeyCode keycode) override;
        virtual bool IsMouseButtonPressedImpl(MouseCode button) override;
        virtual std::pair<float, float> GetMousePositionImpl() override;
        virtual float GetMouseXImpl() override;
        virtual float GetMouseYImpl() override;
    };
}  // namespace Ethereal
