#pragma once

#include "EventHandler.h"

#include "Logger.h" 


namespace projectSolar
{
	EventHandler::EventHandler(const size_t& threadsNumber = 1) :
		m_workersBarrier(threadsNumber + 1),
		m_masterSemaphore(0),
		m_master(&EventHandler::master, this)
	{
		for (size_t i = 0; i < threadsNumber; i++)
		{
			m_workers.emplace_back(&EventHandler::worker, this);
		}
	}
	EventHandler::~EventHandler()
	{
		m_killThreads = true;
		m_masterSemaphore.release();
		for (size_t i = 0; i < m_workers.size(); i++)
		{
			m_workers[i].join();
		}
		m_master.join();
	}

	void EventHandler::receive(EventFunc command, void* data)
	{
		m_events.push({ command, data });
		processEvents();
	}

	void EventHandler::processEvents()
	{
		m_masterSemaphore.release();
	}
	void EventHandler::master()
	{
		while (!m_killThreads)
		{
			m_masterSemaphore.acquire();

			if (m_killThreads)
			{
				m_workersBarrier.arrive_and_wait();
				m_workersBarrier.arrive_and_wait();
				break;
			}

			m_events.swap();

			while (!m_events.empty())
			{
				m_workersBarrier.arrive_and_wait();
				m_workersBarrier.arrive_and_wait();
				m_events.swap();
			}
		}
	}
	void EventHandler::worker()
	{
		while (!m_killThreads)
		{
			m_workersBarrier.arrive_and_wait();

			while (!m_events.empty())
			{
				Event ev = m_events.pop();
				ev.command(this, ev.data);
			}

			m_workersBarrier.arrive_and_wait();
		}
	}
}
