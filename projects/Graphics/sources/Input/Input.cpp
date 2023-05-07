#include "pch.h"

#include "Input.h"
#include "ApplicationEvent.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "Window.h"


namespace projectSolar
{
	// InputEventsManager
	InputEventsManager::InputEventsManager(Window* window) :
		m_window(window)
	{
	}
	InputEvent* InputEventsManager::front()
	{
		return m_events.front();
	}
	void InputEventsManager::push(InputEvent* inputEvent)
	{
		m_events.push(inputEvent);
	}
	void InputEventsManager::pop()
	{
		delete(m_events.front());
		m_events.pop();
	}
	size_t InputEventsManager::size()
	{
		return m_events.size();
	}
	bool InputEventsManager::isEmpty()
	{
		return m_events.empty();
	}
	void InputEventsManager::setupCallbacks()
	{
		glfwSetWindowSizeCallback(m_window->getNativeWindow(), [](GLFWwindow* window, int width, int height)
			{
				WindowProperties& data = *(WindowProperties*)glfwGetWindowUserPointer(window);
				data.width = width;
				data.height = height;

				WindowResizeEvent* inputEvent = new WindowResizeEvent(width, height);
				data.eventsManager->push((InputEvent*)inputEvent);
			});

		glfwSetWindowCloseCallback(m_window->getNativeWindow(), [](GLFWwindow* window)
			{
				WindowProperties& data = *(WindowProperties*)glfwGetWindowUserPointer(window);

				WindowCloseEvent* inputEvent = new WindowCloseEvent;
				data.eventsManager->push((InputEvent*)inputEvent);
			});

		glfwSetKeyCallback(m_window->getNativeWindow(), [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowProperties& data = *(WindowProperties*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent* inputEvent = new KeyPressedEvent(key, 0);
					data.eventsManager->push((InputEvent*)inputEvent);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent* inputEvent = new KeyReleasedEvent(key);
					data.eventsManager->push((InputEvent*)inputEvent);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent* inputEvent = new KeyPressedEvent(key, true);
					data.eventsManager->push((InputEvent*)inputEvent);
					break;
				}
				}
			});

		glfwSetCharCallback(m_window->getNativeWindow(), [](GLFWwindow* window, unsigned int keycode)
			{
				WindowProperties& data = *(WindowProperties*)glfwGetWindowUserPointer(window);

				KeyTypedEvent* inputEvent = new KeyTypedEvent(keycode);
				data.eventsManager->push((InputEvent*)inputEvent);
			});

		glfwSetMouseButtonCallback(m_window->getNativeWindow(), [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowProperties& data = *(WindowProperties*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent* inputEvent = new MouseButtonPressedEvent(button);
					data.eventsManager->push((InputEvent*)inputEvent);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent* inputEvent = new MouseButtonReleasedEvent(button);
					data.eventsManager->push((InputEvent*)inputEvent);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_window->getNativeWindow(), [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowProperties& data = *(WindowProperties*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent* inputEvent = new MouseScrolledEvent((float)xOffset, (float)yOffset);
				data.eventsManager->push((InputEvent*)inputEvent);
			});

		glfwSetCursorPosCallback(m_window->getNativeWindow(), [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowProperties& data = *(WindowProperties*)glfwGetWindowUserPointer(window);

				MouseMovedEvent* inputEvent = new MouseMovedEvent((float)xPos, (float)yPos);
				data.eventsManager->push((InputEvent*)inputEvent);
			});
	}


	// InputManager
	InputManager::InputManager(Window* window) :
		m_window(window)
	{
	}
	bool InputManager::isKeyPressed(KeyCode key)
	{
		GLFWwindow* window = m_window->getNativeWindow();
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS;
	}
	bool InputManager::isMouseButtonPressed(MouseCode button)
	{
		GLFWwindow* window = m_window->getNativeWindow();
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
		return false;
	}
	Eigen::Vector2f InputManager::GetMousePosition()
	{
		GLFWwindow* window = m_window->getNativeWindow();
		double xPosition;
		double yPosition;
		glfwGetCursorPos(window, &xPosition, &yPosition);
		
		return Eigen::Vector2f((float)xPosition, (float)yPosition);
	}

	// InputEventDispatcher

	// Other
	inline std::ostream& operator<<(std::ostream& os, const InputEvent& inputEvent)
	{
		return os << inputEvent.toString();
	}
}
