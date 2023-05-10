#pragma once

#include "EventHandler.h"

#include <queue>
#include <atomic>
#include <functional>
#include <shared_mutex>
#include <unordered_map>
#include <semaphore>
#include <barrier>

#define SUBSCRIBE(receiverClass, receiverPtr, eventId, command) SubscriptionManager::subscribe(SubscriptionManager::eventId, receiverPtr, [](void* receiver, void* data) {\
		((receiverClass*)receiver)->command((receiverClass::PPCAT(command, _DATA)*)data);\
		delete((receiverClass::PPCAT(command, _DATA)*)data);})
#define EVENT_DECL(eventId, ...) enum : uint16_t {eventId = __LINE__}; struct PPCAT(eventId, _DATA) {__VA_ARGS__;}
#define EVENT_DATA_COPY(eventId) case eventId: return new PPCAT(eventId, _DATA)(*(PPCAT(eventId, _DATA)*)data)
#define EMIT_EVENT(eventId, ...) auto* PPCAT(data, __LINE__) = new SubscriptionManager::PPCAT(eventId, _DATA)({__VA_ARGS__}); SubscriptionManager::receive(SubscriptionManager::eventId, PPCAT(data, __LINE__), sizeof(SubscriptionManager::PPCAT(eventId, _DATA)))

namespace projectSolar
{
	class SubscriptionManager
	{
	public:
		SubscriptionManager(const SubscriptionManager&) = delete;
		SubscriptionManager& operator= (const SubscriptionManager&) = delete;

		static void init(const size_t& threadsNumber);
		static void subscribe(uint16_t eventId, void* receiver, EventHandler::EventFunc command);
		static void unsubscribe(void* receiver, uint16_t eventId);
		static void receive(uint16_t eventId, void* data, const size_t& size);

		// *** Events
		enum : uint16_t { UNKNOWN_EVENT = 0 };
		EVENT_DECL(DEBUG_MESSAGE, std::string message);
		
		// ***

	private:
		SubscriptionManager();
		~SubscriptionManager();
		static SubscriptionManager& get();
		
		inline static std::atomic<bool> s_initialized = false;
		inline static size_t s_threadsNumber = 1;
		
		// *** Workers
		std::barrier<std::_No_completion_function> m_workersBarrier;
		std::counting_semaphore<1> m_masterSemaphore;
		std::thread m_master;
		std::vector<std::thread> m_workers;
		bool m_killThreads = false;

		void processEvents();
		void master();
		void worker();
		// ***

		// *** Subscriptions
		struct Subsctibtion
		{
			uint16_t eventId = 0;
			void* data = nullptr;
			size_t size = 0;
		};
		DoubleBufferedQueue<Subsctibtion> m_subsciptionsQueue;
		std::unordered_map<uint16_t, std::shared_mutex> m_mutexes;
		std::unordered_map<uint16_t, std::unordered_map<void*, EventHandler::EventFunc>> m_subscriptions;
		
		void subscribeImpl(uint16_t eventId, void* receiver, EventHandler::EventFunc command);
		void unsubscribeImpl(void* receiver, uint16_t eventId);
		void receiveImpl(uint16_t eventId, void* data, const size_t& length);
		void* copyData(const uint16_t& eventId, void* data);
		// ***
	};
}