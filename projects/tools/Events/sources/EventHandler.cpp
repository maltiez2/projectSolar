#include "EventHandler.h"

#include "Logger.h" 


namespace projectSolar
{
	EventHandler::EventHandler(const size_t& threadsNumber = 1)
	{
		for (size_t i = 0; i < threadsNumber; i++)
		{
			m_workersSemaphores.emplace_back(new std::binary_semaphore(0));
			m_workers.emplace_back(&EventHandler::worker, this, i);
		}
	}
	EventHandler::~EventHandler()
	{
		m_killThreads = true;
		realeseWorkers();
		for (size_t i = 0; i < m_workers.size(); i++)
		{
			m_workers[i].join();
		}
		for (std::binary_semaphore* semaphore : m_workersSemaphores)
		{
			delete(semaphore);
		}
	}

	void EventHandler::receive(EventFunc command, void* data)
	{
		m_events.push({ command, data });
		realeseWorkers();
	}

	void EventHandler::realeseWorkers()
	{
		for (size_t i = 0; i < m_workersSemaphores.size(); i++)
		{
			m_workersSemaphores[i]->release();
		}
	}
	void EventHandler::worker(size_t id)
	{
		while (!m_killThreads)
		{
			while (!m_events.empty())
			{
				Event ev = m_events.pop();
				if (ev.command != nullptr)
				{
					ev.command(this, ev.data);
				}
			}

			if (m_events.emptyAfterSwapIfEmpty())
			{
				m_workersSemaphores[id]->acquire();
			}
			else
			{
				realeseWorkers();
			}
		}
	}
}
