#pragma once

#include <memory>
#include <shared_mutex>
#include <semaphore>
#include <vector>
#include <array>

#include "EventHandlerMacros.h"


namespace projectSolar::Events
{
	constexpr uint8_t MAX_EVENT_ID = 255;
	constexpr uint8_t MAX_EVENT_DATA_SIZE = 255;

	namespace RingBuffer
	{
		constexpr size_t MAX_BUFFER_SIZE = 1i64 << 25; // 16 Mb

		struct Buffer
		{
			uint8_t* front;
			uint8_t* back;
			uint8_t* head;
			uint8_t* tail;
		};

		Buffer* create(const size_t size);
		void destroy(Buffer* buffer);
		void clear(Buffer* buffer, std::shared_mutex* head, std::shared_mutex* tail);

		// Only POD data
		void put(Buffer* buffer, const uint8_t* data, const uint8_t dataType, const uint8_t size, std::shared_mutex* head, std::shared_mutex* tail);
		bool pop(Buffer* buffer, uint8_t* data, uint8_t& dataType, uint8_t& size, std::shared_mutex* head, std::shared_mutex* tail);

		bool splited(const Buffer* buffer, std::shared_mutex* head, std::shared_mutex* tail);
		size_t left(const Buffer* buffer, std::shared_mutex* head, std::shared_mutex* tail);
		bool empty(const Buffer* buffer, std::shared_mutex* head, std::shared_mutex* tail);
		bool full(const Buffer* buffer, std::shared_mutex* head, std::shared_mutex* tail);
	}

	// Only POD data in events
	class EventHandler
	{
	public:
		EventHandler(const size_t& workersNumber = 1, const size_t& bufferSize = RingBuffer::MAX_BUFFER_SIZE);
		virtual ~EventHandler();

		// Accepts only POD as 'data'!
		template<typename DataType>
		void receiveEvent(uint8_t eventType, DataType& data)
		{
			uint8_t size = sizeof(DataType);
			RingBuffer::put(m_buffer, (uint8_t*)&data, eventType, sizeof(DataType), &m_headMutex, &m_tailMutex);
			realeseWorkers();
		}

	protected:
		void destroyWorkers();
		virtual void processEvent(uint8_t eventType, uint8_t* data) = 0;

		RingBuffer::Buffer* m_buffer;
		std::shared_mutex m_headMutex;
		std::shared_mutex m_tailMutex;

		std::vector<std::binary_semaphore*> m_workersSemaphores;
		std::vector<std::thread> m_workers;
		bool m_killThreads = false;

		void realeseWorkers();
		void worker(size_t id);
	};

	// Only POD data in events
	class SubscriptionManager : public EventHandler
	{
	public:
		SubscriptionManager(const size_t& workersNumber = 1, const size_t& bufferSize = RingBuffer::MAX_BUFFER_SIZE);
		~SubscriptionManager() override;

		template<typename SubscriberType>
		void subscribe(uint8_t eventId, const std::shared_ptr<SubscriberType>& subscriber)
		{
			std::unique_lock writeLock(m_subsritpionMutex);
			m_subscriptions[eventId].push_back(std::dynamic_pointer_cast<EventHandler>(subscriber));
		}
		template<typename SubscriberType>
		void unsubscribe(uint8_t eventId, const std::shared_ptr<SubscriberType>& subscriber)
		{
			std::unique_lock writeLock(m_subsritpionMutex);
			std::shared_ptr<EventHandler> ptr = std::dynamic_pointer_cast<EventHandler>(subscriber);
			for (size_t index = 0; index < m_subscriptions[eventId].size(); index++)
			{
				while (m_subscriptions[eventId][index] == ptr && m_subscriptions[eventId].size() > index)
				{
					m_subscriptions[eventId].erase(m_subscriptions[eventId].begin() + index);
				}
			}
		}

	protected:
		std::array<std::vector<std::shared_ptr<EventHandler>>, MAX_EVENT_ID + 1> m_subscriptions;
		std::shared_mutex m_subsritpionMutex;
	};
}
