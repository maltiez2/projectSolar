#include "pch.h"

#include "Event.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "ApplicationEvent.h"
#include "KeyCodes.h"
#include "MouseCodes.h"


using namespace projectSolar;

// InputEvent
std::string InputEvent::toString() const
{
	return getName();
}
bool InputEvent::isInCategory(InputEventCategory category) const
{
	return getCategoryFlags() & category;
}


// APPLICATION
// WindowResizeEvent
projectSolar::WindowResizeEvent::WindowResizeEvent(uint32_t width, uint32_t height) :
	m_width(width),
	m_height(height)
{
}
uint32_t projectSolar::WindowResizeEvent::getWidth() const
{
	return m_width;
}
uint32_t projectSolar::WindowResizeEvent::getHeight() const
{
	return m_height;
}
std::string projectSolar::WindowResizeEvent::toString() const
{
	std::stringstream ss;
	ss << "WindowResizeEvent: " << m_width << ", " << m_height;
	return ss.str();
}
EVENT_CLASS_TYPE_IMPLEMENTATION(WindowResizeEvent, WindowResize)
EVENT_CLASS_CATEGORY_IMPLEMENTATION(WindowResizeEvent, EventCategoryApplication)

// WindowCloseEvent
EVENT_CLASS_TYPE_IMPLEMENTATION(WindowCloseEvent, WindowClose)
EVENT_CLASS_CATEGORY_IMPLEMENTATION(WindowCloseEvent, EventCategoryApplication)


// MOUSE
// MouseMovedEvent
MouseMovedEvent::MouseMovedEvent(const float x, const float y) :
	m_mouseX(x),
	m_mouseY(y)
{
}
float MouseMovedEvent::getX() const
{
	return m_mouseX;
}
float MouseMovedEvent::getY() const
{
	return m_mouseY;
}
std::string MouseMovedEvent::toString() const
{
	std::stringstream ss;
	ss << "MouseMovedEvent: " << m_mouseX << ", " << m_mouseY;
	return ss.str();
}
EVENT_CLASS_TYPE_IMPLEMENTATION(MouseMovedEvent, MouseMoved)
EVENT_CLASS_CATEGORY_IMPLEMENTATION(MouseMovedEvent, EventCategoryMouse | EventCategoryInput)

// MouseScrolledEvent
MouseScrolledEvent::MouseScrolledEvent(const float xOffset, const float yOffset) :
	m_xOffset(xOffset),
	m_yOffset(yOffset)
{
}
float MouseScrolledEvent::getXOffset() const
{
	return m_xOffset;
}
float MouseScrolledEvent::getYOffset() const
{
	return m_yOffset;
}
std::string MouseScrolledEvent::toString() const
{
	std::stringstream ss;
	ss << "MouseScrolledEvent: " << getXOffset() << ", " << getYOffset();
	return ss.str();
}
EVENT_CLASS_TYPE_IMPLEMENTATION(MouseScrolledEvent, MouseScrolled)
EVENT_CLASS_CATEGORY_IMPLEMENTATION(MouseScrolledEvent, EventCategoryMouse | EventCategoryInput)

// MouseButtonEvent
projectSolar::MouseCode MouseButtonEvent::getMouseButton() const
{
	return m_button;
}
MouseButtonEvent::MouseButtonEvent(const projectSolar::MouseCode button) :
	m_button(button)
{
}
EVENT_CLASS_CATEGORY_IMPLEMENTATION(MouseButtonEvent, EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)

// MouseButtonPressedEvent
MouseButtonPressedEvent::MouseButtonPressedEvent(const MouseCode button)
	: MouseButtonEvent(button)
{
}
std::string MouseButtonPressedEvent::toString() const
{
	std::stringstream ss;
	ss << "MouseButtonPressedEvent: " << m_button;
	return ss.str();
}
EVENT_CLASS_TYPE_IMPLEMENTATION(MouseButtonPressedEvent, MouseButtonPressed)

// MouseButtonReleasedEvent
MouseButtonReleasedEvent::MouseButtonReleasedEvent(const MouseCode button) :
	MouseButtonEvent(button)
{
}
std::string MouseButtonReleasedEvent::toString() const
{
	std::stringstream ss;
	ss << "MouseButtonReleasedEvent: " << m_button;
	return ss.str();
}
EVENT_CLASS_TYPE_IMPLEMENTATION(MouseButtonReleasedEvent, MouseButtonReleased)


// KEYBOARD
// KeyEvent
KeyCode KeyEvent::getKeyCode() const
{
	return m_keyCode;
}
explicit KeyEvent::KeyEvent(const KeyCode keycode) :
	m_keyCode(keycode)
{
}
EVENT_CLASS_CATEGORY_IMPLEMENTATION(KeyEvent, EventCategoryKeyboard | EventCategoryInput)

// KeyPressedEvent
KeyPressedEvent::KeyPressedEvent(const KeyCode keycode, bool isRepeat = false) :
	KeyEvent(keycode),
	m_isRepeat(isRepeat)
{
}
bool KeyPressedEvent::isRepeat() const
{
	return m_isRepeat;
}
std::string KeyPressedEvent::toString() const 
{
	std::stringstream ss;
	ss << "KeyPressedEvent: " << m_keyCode << " (repeat = " << m_isRepeat << ")";
	return ss.str();
}
EVENT_CLASS_TYPE_IMPLEMENTATION(KeyPressedEvent, KeyPressed)

// KeyReleasedEvent
KeyReleasedEvent::KeyReleasedEvent(const KeyCode keycode) :
	KeyEvent(keycode)
{
}
std::string KeyReleasedEvent::toString() const
{
	std::stringstream ss;
	ss << "KeyReleasedEvent: " << m_keyCode;
	return ss.str();
}
EVENT_CLASS_TYPE_IMPLEMENTATION(KeyReleasedEvent, KeyReleased)

// KeyTypedEvent
KeyTypedEvent::KeyTypedEvent(const KeyCode keycode) :
	KeyEvent(keycode)
{
}
std::string KeyTypedEvent::toString() const 
{
	std::stringstream ss;
	ss << "KeyTypedEvent: " << m_keyCode;
	return ss.str();
}
EVENT_CLASS_TYPE_IMPLEMENTATION(KeyTypedEvent, KeyTyped)
