#pragma once

#pragma once

#include <functional>

#define BIT(x) (1 << (x))
#define BIND_EVENT_FUNC(x) std::bind(&x, this, std::placeholders::_1)
#define EVENT_CLASS_TYPE_DECLARATION(type) static InputEventType getStaticType();\
								virtual InputEventType getEventType() const override;\
								virtual const char* getName() const override;
#define EVENT_CLASS_TYPE_IMPLEMENTATION(eventClass, type) InputEventType eventClass::getStaticType() { return InputEventType::type; }\
								InputEventType eventClass::getEventType() const { return getStaticType(); }\
								const char* eventClass::getName() const { return #type; }

#define EVENT_CLASS_CATEGORY_DECLARATION(category) virtual int getCategoryFlags() const override;
#define EVENT_CLASS_CATEGORY_IMPLEMENTATION(eventClass, category) int eventClass::getCategoryFlags() const { return category; }


namespace projectSolar::Graphics
{
	enum class InputEventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum InputEventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};


	class InputEvent
	{
	public:
		virtual ~InputEvent() = default;

		bool handled = false;

		virtual InputEventType getEventType() const = 0;
		virtual const char* getName() const = 0;
		virtual int getCategoryFlags() const = 0;
		virtual std::string toString() const;

		bool isInCategory(InputEventCategory category) const;
	};
}