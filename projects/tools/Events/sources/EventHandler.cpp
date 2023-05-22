#pragma once

#include "EventHandler.h"
#include "Logger.h"
#include "RingBuffer.h"

#include <cassert>
#include <string>


namespace projectSolar::Events
{
	// EVENT HANDLER
	EventHandler::EventHandler(const size_t& workersNumber, const size_t& bufferSize)
	{
		m_buffer = DataStructures::RingBuffer::create(bufferSize);

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
		DataStructures::RingBuffer::destroy(m_buffer);
		for (auto* semaphore : m_workersSemaphores)
		{
			delete(semaphore);
		}
	}
	void EventHandler::destroyWorkers()
	{
		m_killThreads = true;
		DataStructures::RingBuffer::clear(m_buffer, &m_headMutex, &m_tailMutex);
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
			while (DataStructures::RingBuffer::pop(m_buffer, data, eventType, dataSize, &m_headMutex, &m_tailMutex))
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
