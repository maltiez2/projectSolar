#pragma once

#include "EventHandler.h"

#include "Logger.h" 

#include <queue>
#include <mutex>
#include <thread>
#include <shared_mutex>
#include <vector>


namespace projectSolar
{
	// EventQueue
	void EventHandler::EventQueue::push(const Event& ev)
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
	EventHandler::Event EventHandler::EventQueue::pop()
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
	void EventHandler::EventQueue::swap()
	{
		std::unique_lock lock(m_stateMutex);
		m_state = !m_state;
	}
	bool EventHandler::EventQueue::empty() const
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
	size_t EventHandler::EventQueue::size() const
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

	// EventHandler
	EventHandler::EventHandler(const size_t& threadsNumber = 1) :
		m_threadsNumber(threadsNumber)
	{
		LOG_ASSERT(threadsNumber > 0, "[EventHandler] Number of threads should be greater than 0");
	}
	EventHandler::~EventHandler()
	{
		for (Event ev = m_events.pop(); ev.sender != nullptr; ev = m_events.pop())
		{
			delete(ev.data);
		}
		m_events.swap();
		for (Event ev = m_events.pop(); ev.sender != nullptr; ev = m_events.pop())
		{
			delete(ev.data);
		}
	}

	void EventHandler::receive(void* sender, EventFunc command, void* data)
	{
		LOG_ASSERT(command != UNKNOWN_COMMAND, "[EventHandler] UNKNOWN_COMMAND was received")
			LOG_DEBUG("[EventHandler] receive - command: ", (int)command);
		m_events.push({ sender, command, data });
		tryStartProcessing();
	}

	void EventHandler::tryStartProcessing()
	{
		bool notProcessing = m_processing.try_lock();
		if (notProcessing)
		{
			m_processing.unlock();
			std::jthread processingWorker(&EventHandler::startProcessing, this);
		}
	}
	void EventHandler::startProcessing()
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
	void EventHandler::process()
	{
		for (Event ev = m_events.pop(); ev.sender != nullptr; ev = m_events.pop())
		{
			auto command = ev.command;
			(this->*command)(ev.sender, ev.data);
			delete(ev.data);
		}
	}
}
