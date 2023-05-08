#pragma once

#include "Simulation.h"
#include "Graphics.h"
#include "Logger.h"

#include <atomic>
#include <queue>
#include <vector>
#include <functional>
#include <thread>

namespace projectSolar
{
	void initial(void* manager, void* data);
	void closeWindow(void* manager, void* data);
	
	using TaskFunc = std::function<void(void*, void*)>;

	struct Task
	{
		TaskFunc task;
		void* data;
	};

	class TaskQueue
	{
	public:
		TaskQueue() = default;
		~TaskQueue() = default;

		void push(TaskFunc task, void* data)
		{
			if (m_state)
			{
				m_tasks_0.emplace(task, data);
			}
			else
			{
				m_tasks_1.emplace(task, data);
			}
		}
		Task pop()
		{
			if (m_state)
			{
				Task task = m_tasks_1.front();
				m_tasks_1.pop();
				return task;
			}
			else
			{
				Task task = m_tasks_0.front();
				m_tasks_0.pop();
				return task;
			}
		}
		void swap()
		{
			m_state = !m_state;
		}
		bool empty() const
		{
			if (m_state)
			{
				return m_tasks_1.empty();
			}
			else
			{
				return m_tasks_0.empty();
			}
		}

	private:
		std::queue<Task> m_tasks_0;
		std::queue<Task> m_tasks_1;
		bool m_state;
	};
	
	class TaskManager
	{
	public:
		Simulation::SimulationRunner* simulation;
		Graphics::GuiWindowsManager* windows;
		Graphics::Window* window;


		explicit TaskManager(Simulation::SimulationRunner* simulation, Graphics::GuiWindowsManager* windows, Graphics::Window* window) :
			simulation(simulation),
			windows(windows),
			window(window)
		{
			current().push(initial, nullptr);
			current().swap();
		}
		~TaskManager()
		{
			while (!current().empty())
			{
				delete(current().pop().data);
			}
			current().swap();
			while (!current().empty())
			{
				delete(current().pop().data);
			}
			swap();
			while (!current().empty())
			{
				delete(current().pop().data);
			}
			current().swap();
			while (!current().empty())
			{
				delete(current().pop().data);
			}
		}

		void pushCurrent(TaskFunc task, void* data)
		{
			current().push(task, data);
		}
		void pushFuture(TaskFunc task, void* data)
		{
			future().push(task, data);
		}

		void execute()
		{
			while (!current().empty())
			{
				while (!current().empty())
				{
					Task task = current().pop();
					task.task(this, task.data);
					delete(task.data);
				}
				current().swap();
			}
			swap();
			current().swap();
		}

	private:
		void swap()
		{
			m_state = !m_state;
		}
		TaskQueue& current()
		{
			if (m_state)
			{
				return m_tasks_0;
			}
			else
			{
				return m_tasks_1;
			}
		}
		TaskQueue& future()
		{
			if (m_state)
			{
				return m_tasks_1;
			}
			else
			{
				return m_tasks_0;
			}
		}
		
		TaskQueue m_tasks_0;
		TaskQueue m_tasks_1;
		bool m_state;
		
	};

	// TASK FUNCTIONS

	void initial(void* manager, void* data)
	{
		LOG_DEBUG("[task] Initial");
		((TaskManager*)manager)->pushCurrent(closeWindow, nullptr);
	}
	void closeWindow(void* manager, void* data)
	{
		((TaskManager*)manager)->pushFuture(closeWindow, nullptr);
		Graphics::DebugWindow* const guiWindow = ((TaskManager*)manager)->windows->get<Graphics::DebugWindow>("debug");
		if (guiWindow->closeApp)
		{
			LOG_DEBUG("[task] closeWindow - close");
			glfwSetWindowShouldClose(((TaskManager*)manager)->window->getNativeWindow(), GL_TRUE);
		}
		else
		{
			LOG_DEBUG("[task] closeWindow - not close");
		}
	}
}
