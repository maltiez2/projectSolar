#pragma once

#include "DoubleBufferedQueue.h"
#include "Logger.h"

#include "ring_buffer/ring_buffer.h"

#include <queue>
#include <thread>
#include <vector>
#include <semaphore>
#include <shared_mutex>
#include <memory>


namespace projectSolar
{
	
	class DoubledRingBuffer
	{
	public:
		explicit DoubledRingBuffer(const size_t& bufferSize);

	private:
		Ring_Buffer m_events_0;
		Ring_Buffer m_events_1;
		mutable std::shared_mutex m_stateMutex;
		bool m_state = false;
	};
	
	class EntityEventHandler
	{
	public:
		EntityEventHandler(const size_t& workersNumber, const size_t& bufferSize);

		struct Event
		{

		};

	private:
		DoubledRingBuffer m_events;

		std::vector<std::unique_ptr<std::binary_semaphore>> m_workersSemaphores;
		std::vector<std::thread> m_workers;
		bool m_killThreads = false;

		void realeseWorkers();
		void worker(size_t id);
	};

}
