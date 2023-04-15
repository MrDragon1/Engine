#pragma once

#include "Event.h"
#include "Base/KeyCodes.h"
namespace Ethereal
{

    class KeyEvent : public Event {
      public:
        inline KeyCode GetKeyCode() const { return mKeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
      protected:
        KeyEvent(KeyCode keycode) : mKeyCode(keycode) {}

        KeyCode mKeyCode;
    };

    class KeyPressedEvent : public KeyEvent {
      public:
        KeyPressedEvent(KeyCode keycode, bool isRepeat = false) : KeyEvent(keycode), mIsRepeat(isRepeat) {}

        bool IsRepeat() const { return mIsRepeat; }

        std::string ToString() const override {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << mKeyCode << " (repeat = " << mIsRepeat << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)
      private:
        bool mIsRepeat;
    };

    class KeyReleasedEvent : public KeyEvent {
      public:
        KeyReleasedEvent(KeyCode keycode) : KeyEvent(keycode) {}

        std::string ToString() const override {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << mKeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public KeyEvent {
      public:
        KeyTypedEvent(KeyCode keycode) : KeyEvent(keycode) {}

        std::string ToString() const override {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << mKeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };
}  // namespace Ethereal