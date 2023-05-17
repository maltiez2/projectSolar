#pragma once

#include "EventHandler.h"
#include "Logger.h"

#include <cassert>
#include <string>


namespace projectSolar::Events
{
	namespace RingBuffer
	{
		Buffer* create(const size_t size) // size in powers of 2
		{
			LOG_ASSERT(size <= MAX_BUFFER_SIZE, "Buffers size: ", size, " - greater than max size : ", MAX_BUFFER_SIZE);

			Buffer* buffer = (Buffer*)malloc(sizeof(Buffer));

			LOG_ASSERT(buffer != nullptr, "Unsuccessful buffer struct 'malloc'");

			buffer->front = (uint8_t*)malloc(size + 1);

			LOG_ASSERT(buffer->front != nullptr, "Unsuccessful buffer 'malloc'");

			buffer->back = buffer->front + size + 1;

			buffer->head = buffer->front;
			buffer->tail = buffer->front;

			return buffer;
		}
		void destroy(Buffer* buffer)
		{
			LOG_ASSERT(buffer, "NULL buffer");

			free(buffer->front);
			free(buffer);
		}
		void clear(Buffer* buffer, std::shared_mutex* head, std::shared_mutex* tail)
		{
			std::unique_lock hLock(*head);
			std::unique_lock tLock(*tail);
			buffer->head = buffer->front;
			buffer->tail = buffer->front;
		}

		// 'buffer->head != buffer->tail' on not empty should be enforced by size constrains inside these functions:
		void put(Buffer* buffer, const uint8_t* data, const uint8_t dataType, const uint8_t size, std::shared_mutex* head, std::shared_mutex* tail)
		{
			LOG_ASSERT(buffer, "NULL buffer");
			LOG_ASSERT(data, "NULL input");
			LOG_ASSERT(left(buffer, head, tail) > (size + 2), "Not enough space in buffer: ", left(buffer, head, tail), " < ", size + 2);
			if (left(buffer, head, tail) <= (size + 2))
			{
				LOG_WARN("[RingBuffer] Buffer overflow");
				return;
			}

			std::unique_lock hLock(*head);
			std::shared_lock tLock(*tail);
			if (buffer->tail > buffer->head)
			{
				tLock.unlock();
				memcpy(buffer->head, &dataType, 1);
				memcpy(buffer->head + 1, &size, 1);
				memcpy(buffer->head + 2, data, size);
				buffer->head += size + 2;
				return;
			}
			tLock.unlock();

			if (buffer->back - buffer->head > (size + 2))
			{
				memcpy(buffer->head, &dataType, 1);
				memcpy(buffer->head + 1, &size, 1);
				memcpy(buffer->head + 2, data, size);
				buffer->head += size + 2;
				return;
			}

			size_t cut = buffer->back - buffer->head;
			memcpy(buffer->head, &dataType, 1);
			if (cut == 1)
			{
				memcpy(buffer->front, &size, 1);
				memcpy(buffer->front + 1, data, size);
				buffer->front += size + 1;
				return;
			}
			memcpy(buffer->head + 1, &size, 1);
			if (cut == 2)
			{
				memcpy(buffer->front, data, size);
				buffer->front += size;
				return;
			}
			memcpy(buffer->head + 2, data, cut - 2);
			memcpy(buffer->front, data + cut - 2, size - cut + 2);
			buffer->head = buffer->front + (size - cut + 2);
		}
		bool pop(Buffer* buffer, uint8_t* data, uint8_t& dataType, uint8_t& size, std::shared_mutex* head, std::shared_mutex* tail)
		{
			LOG_ASSERT(buffer, "NULL buffer");
			LOG_ASSERT(data, "NULL output");

			std::shared_lock hLock(*head);
			std::unique_lock tLock(*tail);

			if (buffer->tail == buffer->head)
			{
				return false;
			}

			if (buffer->tail <= buffer->head)
			{
				LOG_ASSERT(buffer->head - buffer->tail >= 2, "Buffer size less then header size");
				memcpy(&dataType, buffer->tail, 1);
				memcpy(&size, buffer->tail + 1, 1);
				LOG_ASSERT(buffer->head - buffer->tail >= size + 2, "Buffer size less then data size");
				hLock.unlock();

				memcpy(data, buffer->tail + 2, size);
				buffer->tail += size + 2;
				return true;
			}

			LOG_ASSERT((buffer->back - buffer->head) + (buffer->tail - buffer->head) >= 2, "Buffer size less then header size");
			memcpy(&dataType, buffer->tail, 1);

			if (buffer->back - buffer->tail == 1)
			{
				memcpy(&size, buffer->front, 1);
				LOG_ASSERT(buffer->head - buffer->front >= size + 1, "Buffer size less then data size");
				hLock.unlock();

				memcpy(data, buffer->front + 1, size);
				buffer->tail = buffer->front + size + 1;
				return true;
			}

			if (buffer->back - buffer->tail == 2)
			{
				memcpy(&size, buffer->tail + 1, 1);
				LOG_ASSERT(buffer->head - buffer->front >= size, "Buffer size less then data size");
				hLock.unlock();

				memcpy(data, buffer->front, size);
				buffer->tail = buffer->front + size;
				return true;
			}

			memcpy(&size, buffer->tail + 1, 1);
			LOG_ASSERT((buffer->back - buffer->head) + (buffer->tail - buffer->head) >= size + 2, "Buffer size less then data size");
			hLock.unlock();

			if (buffer->back - buffer->tail > size + 2)
			{
				memcpy(data, buffer->tail + 2, size);
				buffer->tail += size + 2;
				return true;
			}

			size_t cut = buffer->back - buffer->tail - 2;
			memcpy(data, buffer->tail + 2, cut);
			memcpy(data + cut, buffer->front, size - cut);
			buffer->tail = buffer->front + (size - cut);
			return true;
		}

		bool splited(const Buffer* buffer, std::shared_mutex* head, std::shared_mutex* tail)
		{
			LOG_ASSERT(buffer, "NULL buffer");
			std::shared_lock hLock(*head);
			std::shared_lock tLock(*tail);
			return buffer->tail > buffer->head;
		}
		size_t left(const Buffer* buffer, std::shared_mutex* head, std::shared_mutex* tail)
		{
			LOG_ASSERT(buffer, "NULL buffer");
			std::shared_lock hLock(*head);
			std::shared_lock tLock(*tail);
			if (buffer->tail == buffer->head)
			{
				return buffer->back - buffer->front;
			}
			if (buffer->tail > buffer->head)
			{
				return buffer->tail - buffer->head;
			}
			else
			{
				return (buffer->back - buffer->head) + (buffer->tail - buffer->front);
			}
		}
		bool empty(const Buffer* buffer, std::shared_mutex* head, std::shared_mutex* tail)
		{
			LOG_ASSERT(buffer, "NULL buffer");
			std::shared_lock hLock(*head);
			std::shared_lock tLock(*tail);
			return  buffer->head == buffer->tail;
		}
		bool full(const Buffer* buffer, std::shared_mutex* head, std::shared_mutex* tail)
		{
			LOG_ASSERT(buffer, "NULL buffer");
			return left(buffer, head, tail) == 1;
		}
	}

	// EVENT HANDLER
	EventHandler::EventHandler(const size_t& workersNumber, const size_t& bufferSize)
	{
		m_buffer = RingBuffer::create(bufferSize);

		for (size_t i = 0; i < workersNumber; i++)
		{
			m_workersSemaphores.emplace_back(new std::binary_semaphore(0));
		}
		for (size_t i = 0; i < workersNumber; i++)
		{
			m_workers.emplace_back(&EventHandler::worker, this, i);
		}
	}
	EventHandler::~EventHandler()
	{
		LOG_ASSERT(m_killThreads, "[EventHandler] 'destroyWorkers' was not called in derived class destructor");
		RingBuffer::destroy(m_buffer);
		for (auto* semaphore : m_workersSemaphores)
		{
			delete(semaphore);
		}
	}
	void EventHandler::destroyWorkers()
	{
		m_killThreads = true;
		RingBuffer::clear(m_buffer, &m_headMutex, &m_tailMutex);
		realeseWorkers();
		for (size_t i = 0; i < m_workers.size(); i++)
		{
			m_workersSemaphores[i]->release();
			m_workers[i].join();
		}
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
		uint8_t eventType;
		uint8_t dataSize;
		uint8_t data[255];

		while (!m_killThreads)
		{
			while (RingBuffer::pop(m_buffer, data, eventType, dataSize, &m_headMutex, &m_tailMutex))
			{
				processEvent(eventType, data);
			}

			m_workersSemaphores[id]->acquire();
		}
	}

	// SUBSCRIPTION MANAGER
	SubscriptionManager::SubscriptionManager(const size_t& workersNumber, const size_t& bufferSize) :
		EventHandler(workersNumber, bufferSize)
	{
	}
	SubscriptionManager::~SubscriptionManager()
	{
		destroyWorkers();
	}
}
