#include "EventHandlerAdvanced.h"

#include "Logger.h" 


namespace projectSolar
{
	DoubledRingBuffer::DoubledRingBuffer(const size_t& bufferSize) :
		m_events_0(bufferSize),
		m_events_1(bufferSize)
	{
	}


	EntityEventHandler::EntityEventHandler(const size_t& workersNumber, const size_t& bufferSize) :
		m_events(bufferSize)
	{
		for (size_t i = 0; i < workersNumber; i++)
		{
			m_workersSemaphores.push_back(std::move(std::make_unique<std::binary_semaphore>(0)));
			m_workers.emplace_back(&EntityEventHandler::worker, this, i);
		}
	}

	void EntityEventHandler::realeseWorkers()
	{
		for (size_t i = 0; i < m_workersSemaphores.size(); i++)
		{
			m_workersSemaphores[i]->release();
		}
	}

	void EntityEventHandler::worker(size_t id)
	{
		Event ev;
		
		while (!m_killThreads)
		{
			/*while (m_events.pop(ev))
			{
				// DO STUFF
			}*/

			m_workersSemaphores[id]->acquire();
		}
	}

}

