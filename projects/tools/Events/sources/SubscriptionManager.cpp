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
	void SubscriptionManager::receive(uint16_t eventId, void* data, const size_t& size)
	{
		get().m_subsciptionsQueue.push({ eventId, data, size });
		get().processEvents();
	}
	SubscriptionManager& SubscriptionManager::get()
	{
		static SubscriptionManager instance;
		return instance;
	}

	SubscriptionManager::SubscriptionManager() :
		m_workersBarrier(s_threadsNumber + 1),
		m_masterSemaphore(0),
		m_master(&SubscriptionManager::master, this)
	{
		for (size_t i = 0; i < s_threadsNumber; i++)
		{
			m_workers.emplace_back(&SubscriptionManager::worker, this);
		}
	}
	SubscriptionManager::~SubscriptionManager()
	{
		for (Subsctibtion subscription = m_subsciptionsQueue.pop(); subscription.eventId != UNKNOWN_EVENT; subscription = m_subsciptionsQueue.pop())
		{
			delete(subscription.data);
		}
		m_subsciptionsQueue.swap();
		for (Subsctibtion subscription = m_subsciptionsQueue.pop(); subscription.eventId != UNKNOWN_EVENT; subscription = m_subsciptionsQueue.pop())
		{
			delete(subscription.data);
		}

		m_killThreads = true;
		m_masterSemaphore.release();
		for (size_t i = 0; i < m_workers.size(); i++)
		{
			m_workers[i].join();
		}
		m_master.join();
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
	void SubscriptionManager::receiveImpl(uint16_t eventId, void* data, const size_t& size)
	{
		std::shared_lock eventLock(m_mutexes[eventId]);
		for (auto [receiver, command] : m_subscriptions[eventId])
		{
			void* copy = malloc(size);
			if (copy == nullptr)
			{
				LOG_ASSERT(false, "Malloc returned nullptr in 'SubscriptionManager::receiveImpl'");
				continue;
			}
			memcpy(copy, data, size);
			//LOG_WARN("SubscriptionManager::receiveImpl - send command: ", eventId);
			SEND_COMMAND((EventHandler*)receiver, command, copy);
		}
		delete(data);
	}

	void SubscriptionManager::processEvents()
	{
		m_masterSemaphore.release();
	}
	void SubscriptionManager::master()
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

			m_subsciptionsQueue.swap();

			while (!m_subsciptionsQueue.empty())
			{
				m_workersBarrier.arrive_and_wait();
				m_workersBarrier.arrive_and_wait();
				m_subsciptionsQueue.swap();
			}
		}
	}
	void SubscriptionManager::worker()
	{
		while (!m_killThreads)
		{
			m_workersBarrier.arrive_and_wait();
			
			while (!m_subsciptionsQueue.empty())
			{
				Subsctibtion subscription = m_subsciptionsQueue.pop();
				receiveImpl(subscription.eventId, subscription.data, subscription.size);
			}

			m_workersBarrier.arrive_and_wait();
		}
	}
}
