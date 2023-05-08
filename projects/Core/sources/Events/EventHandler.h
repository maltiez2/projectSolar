#pragma once

#include "Events.h"

#include "Simulation.h"
#include "Graphics.h"
#include "Logger.h" 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <queue>
#include <mutex>
#include <thread>
#include <shared_mutex>
#include <vector>


namespace projectSolar
{
	class EventHandler;
	
	struct Event
	{
		EventHandler* sender = nullptr;
		Command command = Command::UNKNOWN_COMMAND;
		void* data = nullptr;
	};

	class EventQueue // Thread safe double buffered queue
	{
	public:
		void push(const Event& ev)
		{
			std::shared_lock stateLock(m_stateMutex);
			if (m_state)
			{
				std::unique_lock lock(m_mutex_0);
				m_events_0.emplace(ev);
			}
			else
			{
				std::unique_lock lock(m_mutex_1);
				m_events_1.emplace(ev);
			}
		}
		Event pop()
		{
			std::shared_lock stateLock(m_stateMutex);
			if (m_state)
			{
				std::unique_lock lock(m_mutex_1);
				if (m_events_1.empty())
				{
					return Event();
				}

				Event ev = m_events_1.front();
				m_events_1.pop();
				return ev;
			}
			else
			{
				std::unique_lock lock(m_mutex_0);
				if (m_events_0.empty())
				{
					return Event();
				}

				Event ev = m_events_0.front();
				m_events_0.pop();
				return ev;
			}
		}
		void swap()
		{
			std::unique_lock lock(m_stateMutex);
			m_state = !m_state;
		}
		bool empty() const
		{
			std::shared_lock stateLock(m_stateMutex);
			if (m_state)
			{
				std::shared_lock lock(m_mutex_1);
				return m_events_1.empty();
			}
			else
			{
				std::shared_lock lock(m_mutex_0);
				return m_events_0.empty();
			}
		}
		size_t size() const
		{
			std::shared_lock stateLock(m_stateMutex);
			if (m_state)
			{
				std::shared_lock lock(m_mutex_1);
				return m_events_1.size();
			}
			else
			{
				std::shared_lock lock(m_mutex_0);
				return m_events_0.size();
			}
		}

	private:
		std::queue<Event> m_events_0;
		std::queue<Event> m_events_1;
		mutable std::shared_mutex m_mutex_0;
		mutable std::shared_mutex m_mutex_1;
		mutable std::shared_mutex m_stateMutex;
		bool m_state;
	};
	
	class EventHandler
	{
	public:
		EventHandler(void* owner, const size_t& threadsNumber = 1) :
			m_owner(owner),
			m_threadsNumber(threadsNumber)
		{
			LOG_ASSERT(threadsNumber > 0, "[EventHandler] Number of threads should be greater than 0");
		}
		virtual ~EventHandler()
		{
			for (Event ev = m_events.pop(); ev.command != Command::UNKNOWN_COMMAND; ev = m_events.pop())
			{
				delete(ev.data);
			}
			m_events.swap();
			for (Event ev = m_events.pop(); ev.command != Command::UNKNOWN_COMMAND; ev = m_events.pop())
			{
				delete(ev.data);
			}
		}

		void send(EventHandler* desctination, const Command& command, void* data)
		{
			LOG_ASSERT(command != Command::UNKNOWN_COMMAND, "[EventHandler] UNKNOWN_COMMAND was sent")
			desctination->receive(this, command, data);
		}
		void receive(EventHandler* sender, const Command& command, void* data)
		{
			LOG_ASSERT(command != Command::UNKNOWN_COMMAND, "[EventHandler] UNKNOWN_COMMAND was received")
			LOG_DEBUG("[EventHandler] receive - command: ", (int)command);
			m_events.push({ sender, command, data });
			tryStartProcessing();
		}


	protected:
		void* m_owner;
		size_t m_threadsNumber;
		EventQueue m_events;
		std::mutex m_processing;

		void tryStartProcessing()
		{
			bool notProcessing = m_processing.try_lock();
			if (notProcessing)
			{
				m_processing.unlock();
				std::jthread processingWorker(&EventHandler::startProcessing, this);
			}
		}
		void startProcessing()
		{
			bool notProcessing = m_processing.try_lock();
			if (notProcessing)
			{
				m_events.swap();
				std::vector<std::jthread> threads;
				for (size_t i = 0; i < m_threadsNumber; i++)
				{
					threads.emplace_back(&EventHandler::process, this);
				}
				m_processing.unlock();
			}
		}
		void process()
		{
			for (Event ev = m_events.pop(); ev.command != Command::UNKNOWN_COMMAND; ev = m_events.pop())
			{
				processEvent(ev);
				delete(ev.data);
			}
		}

		virtual void processEvent(const Event& ev) = 0;
	};

	class WindowHandler : public EventHandler
	{
	public:
		WindowHandler(void* owner, const size_t& threadsNumber = 1) :
			EventHandler(owner, threadsNumber)
		{
		}
		~WindowHandler() override = default;


	private:
		void processEvent(const Event& ev) override
		{
			switch (ev.command)
			{
			case Command::WND_WINDOW_CLOSE:
				LOG_DEBUG("Event to close window");
				//glfwSetWindowShouldClose(((Window*)m_owner)->getNativeWindow(), GL_TRUE);
				break;
			default:
				LOG_ASSERT(false, "[ApplicationHandler] Unknown command: ", (uint8_t)ev.command)
			}
		}
	};

	class ApplicationHandler : public EventHandler
	{
	public:
		ApplicationHandler(void* owner, const size_t& threadsNumber = 1) :
			EventHandler(owner, threadsNumber)
		{
		}
		~ApplicationHandler() override = default;


	private:
		void processEvent(const Event& ev) override
		{
			switch (ev.command)
			{
			case Command::APP_SET_MAP_VIEW:
				LOG_DEBUG("APP_SET_MAP_VIEW");
				PROCESS_EVENT(APP_SET_MAP_VIEW, ev.data);
				break;
			default:
				LOG_ASSERT(false, "[ApplicationHandler] Unknown command: ", (uint8_t)ev.command)
			}
		}
	};
}
