#pragma once

#include <queue>
#include <mutex>
#include <functional>
#include <shared_mutex>

#define PPCAT_NX(A, B) A ## B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define SLOT_DECL(command, ...) enum : uint16_t { command = __LINE__ }; struct PPCAT(command, _DATA) {__VA_ARGS__;}; void PPCAT(command, _FUNC)(void* sender, PPCAT(command, _DATA)* data)
#define SLOT_IMPL(command, namespc) void namespc::PPCAT(command, _FUNC)(void* sender, PPCAT(command, _DATA)* data)
#define SLOT_FUNC_PTR(command) std::function<void(void*, PPCAT(command, _DATA)*)>
#define EMIT_EVENT(destClass, dest, command, ...) auto* PPCAT(data, __LINE__) = new destClass::PPCAT(command, _DATA)({__VA_ARGS__}); (dest)->receive((void*)this, (EventHandler::EventFunc)&destClass::PPCAT(command, _FUNC), PPCAT(data, __LINE__))
#define PROCESS_EVENT(sender, command, data) PPCAT(command, _FUNC)(sender, (PPCAT(command, _DATA)*)(data))

namespace projectSolar
{
	class EventHandler
	{
	public:
		using EventFunc = void(EventHandler::*)(void*, void*);

		struct Event
		{
			void* sender = nullptr;
			EventHandler::EventFunc command = nullptr;
			void* data = nullptr;
		};

		class EventQueue
		{
		public:
			void push(const Event& ev);
			Event pop();
			void swap();
			bool empty() const;
			size_t size() const;

		private:
			std::queue<Event> m_events_0;
			std::queue<Event> m_events_1;
			mutable std::shared_mutex m_mutex_0;
			mutable std::shared_mutex m_mutex_1;
			mutable std::shared_mutex m_stateMutex;
			bool m_state;
		};
		
		enum : uint16_t
		{
			UNKNOWN_COMMAND = 0
		};
		
		explicit EventHandler(const size_t& threadsNumber);
		virtual ~EventHandler();

		void receive(void* sender, EventFunc command, void* data);
	
	private:
		size_t m_threadsNumber;
		EventQueue m_events;
		std::mutex m_processing;

		void tryStartProcessing();
		void startProcessing();
		void process();
	};
}
