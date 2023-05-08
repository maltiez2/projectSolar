#pragma once

#include "Event.h"
#include "MouseCodes.h"

#include <sstream>

namespace projectSolar::Graphics
{

	class MouseMovedEvent : public InputEvent
	{
	public:
		MouseMovedEvent(const float x, const float y);

		float getX() const;
		float getY() const;

		std::string toString() const override;


		EVENT_CLASS_TYPE_DECLARATION(MouseMoved)
		EVENT_CLASS_CATEGORY_DECLARATION(EventCategoryMouse | EventCategoryInput)
	private:
		float m_mouseX;
		float m_mouseY;
	};

	class MouseScrolledEvent : public InputEvent
	{
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset);

		float getXOffset() const;
		float getYOffset() const;

		std::string toString() const override;

		EVENT_CLASS_TYPE_DECLARATION(MouseScrolled)
		EVENT_CLASS_CATEGORY_DECLARATION(EventCategoryMouse | EventCategoryInput)
	private:
		float m_xOffset;
		float m_yOffset;
	};

	class MouseButtonEvent : public InputEvent
	{
	public:
		MouseCode getMouseButton() const;

		EVENT_CLASS_CATEGORY_DECLARATION(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
	protected:
		explicit MouseButtonEvent(const MouseCode button);

		MouseCode m_button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		explicit MouseButtonPressedEvent(const MouseCode button);

		std::string toString() const override;

		EVENT_CLASS_TYPE_DECLARATION(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const MouseCode button);

		std::string toString() const override;

		EVENT_CLASS_TYPE_DECLARATION(MouseButtonReleased)
	};

}
