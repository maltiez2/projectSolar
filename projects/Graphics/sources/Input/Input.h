#pragma once

#include <queue>
#include <Eigen/Eigen>
#include <memory>

namespace projectSolar::Graphics
{
	class Window;
	class InputEvent;
	
	class InputEventsManager
	{
	public:
		explicit InputEventsManager(Window* window);
		~InputEventsManager() = default;

		InputEvent* front();
		void push(InputEvent* inputEvent);
		void pop();
		size_t size();
		bool isEmpty();

		template<typename T, typename F>
		bool dispatch(const F& func)
		{
			if (m_events.front()->getEventType() == T::getStaticType())
			{
				m_events.front()->handled |= func(static_cast<T&>(m_events.front()));
				return true;
			}
			return false;
		}

		void setupCallbacks();

	private:
		Window* m_window;
		std::queue<InputEvent*> m_events;
	};

	class InputManager
	{
	public:
		explicit InputManager(Window* window);
		~InputManager() = default;

		bool isKeyPressed(uint16_t key);
		bool isMouseButtonPressed(uint16_t button);
		Eigen::Vector2f GetMousePosition();
	private:
		Window* m_window;
	};

	inline std::ostream& operator<<(std::ostream& os, const InputEvent& inputEvent);
}
