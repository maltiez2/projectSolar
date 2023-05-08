#pragma once

#include "Event.h"

#include <sstream>

namespace projectSolar::Graphics
{

	class WindowResizeEvent : public InputEvent
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height);

		uint32_t getWidth() const;
		uint32_t getHeight() const;

		std::string toString() const override;

		EVENT_CLASS_TYPE_DECLARATION(WindowResize)
		EVENT_CLASS_CATEGORY_DECLARATION(EventCategoryApplication)
	private:
		uint32_t m_width;
		uint32_t m_height;
	};

	class WindowCloseEvent : public InputEvent
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE_DECLARATION(WindowClose)
		EVENT_CLASS_CATEGORY_DECLARATION(EventCategoryApplication)
	};
}