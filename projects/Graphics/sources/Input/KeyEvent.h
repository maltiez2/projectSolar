#pragma once

#include "Event.h"
#include "KeyCodes.h"

#include <sstream>

namespace projectSolar::Graphics
{

	class KeyEvent : public InputEvent
	{
	public:
		KeyCode getKeyCode() const;

		EVENT_CLASS_CATEGORY_DECLARATION(EventCategoryKeyboard | EventCategoryInput)
	protected:
		explicit KeyEvent(const KeyCode keycode);

		KeyCode m_keyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(const KeyCode keycode, bool isRepeat = false);

		bool isRepeat() const;

		std::string toString() const override;

		EVENT_CLASS_TYPE_DECLARATION(KeyPressed)
	private:
		bool m_isRepeat;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		explicit KeyReleasedEvent(const KeyCode keycode);

		std::string toString() const override;

		EVENT_CLASS_TYPE_DECLARATION(KeyReleased)
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		explicit KeyTypedEvent(const KeyCode keycode);

		std::string toString() const override;

		EVENT_CLASS_TYPE_DECLARATION(KeyTyped)
	};
}
