#include "SubscriptionManager.h"

#include "Logger.h"

namespace projectSolar
{
	void SubscriptionManager::init(const size_t& threadsNumber)
	{
		if (!s_initialized)
		{
			s_initialized = true;
			s_threadsNumber = threadsNumber;
			get();
		}
	}
	void SubscriptionManager::subscribe(uint16_t eventId, void* receiver, EventHandler::EventFunc command)
	{
		get().subscribeImpl(eventId, receiver, command);
	}
	void SubscriptionManager::unsubscribe(void* receiver, uint16_t eventId)
	{
		get().unsubscribeImpl(receiver, eventId);
	}
	void SubscriptionManager::receive(uint16_t eventId, void* data)
	{
		get().m_subsciptionsQueue.push({ eventId, data });
		get().realeseWorkers();
	}
	SubscriptionManager& SubscriptionManager::get()
	{
		static SubscriptionManager instance;
		return instance;
	}

	void SubscriptionManager::realeseWorkers()
	{
		for (size_t i = 0; i < m_workersSemaphores.size(); i++)
		{
			m_workersSemaphores[i]->release();
		}
	}
	void SubscriptionManager::worker(size_t id)
	{
		while (!m_killThreads)
		{
			while (!m_subsciptionsQueue.empty())
			{
				Subsctibtion subscription = m_subsciptionsQueue.pop();
				if (subscription.eventId != UNKNOWN_EVENT)
				{
					receiveImpl(subscription.eventId, subscription.data);
				}
			}

			if (m_subsciptionsQueue.emptyAfterSwapIfEmpty())
			{
				m_workersSemaphores[id]->acquire();
			}
			else
			{
				realeseWorkers();
			}
		}
	}

	SubscriptionManager::SubscriptionManager()
	{
		for (size_t i = 0; i < s_threadsNumber; i++)
		{
			m_workersSemaphores.emplace_back(new std::binary_semaphore(0));
			m_workers.emplace_back(&SubscriptionManager::worker, this, i);
		}
	}
	SubscriptionManager::~SubscriptionManager()
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

	void SubscriptionManager::subscribeImpl(uint16_t eventId, void* receiver, EventHandler::EventFunc command)
	{
		std::unique_lock eventLock(m_mutexes[eventId]);
		if (!m_subscriptions.contains(eventId))
		{
			m_subscriptions[eventId] = {};
		}
		m_subscriptions[eventId][receiver] = command;
	}
	void SubscriptionManager::unsubscribeImpl(void* receiver, uint16_t eventId)
	{
		std::unique_lock eventLock(m_mutexes[eventId]);
		m_subscriptions[eventId].erase(receiver);
	}
	void SubscriptionManager::receiveImpl(uint16_t eventId, void* data)
	{
		std::shared_lock eventLock(m_mutexes[eventId]);
		size_t left = m_subscriptions[eventId].size();
		LOG_INFO("[SubscriptionManager] Subscribers number: ", left, ", event: ", eventId);
		for (auto [receiver, command] : m_subscriptions[eventId])
		{
			if (--left == 0)
			{
				SEND_COMMAND((EventHandler*)receiver, command, data);
				continue;
			}

			void* copy = copyData(eventId, data);
			SEND_COMMAND((EventHandler*)receiver, command, copy);
		}
	}
	void* SubscriptionManager::copyData(const uint16_t& eventId, void* data)
	{
		switch (eventId)
		{
		EVENT_DATA_COPY(DEBUG_MESSAGE);
		EVENT_DATA_COPY(SIM_DATA_UPDATE);
		
		default:
			LOG_ASSERT(false, "[SubscriptionManager] Copy function is not specified for event: ", eventId);
			return nullptr;
		}
	}
}
