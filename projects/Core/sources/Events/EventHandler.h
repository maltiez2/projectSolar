#pragma once

#include <queue>
#include <mutex>
#include <shared_mutex>

#define PPCAT_NX(A, B) A ## B
#define PPCAT(A, B) PPCAT_NX(A, B)


#define SLOT(command, ...) enum : uint16_t { command = __LINE__ }; struct PPCAT(command, _DATA) {__VA_ARGS__;}; void PPCAT(command, _FUNC)(void* sender, PPCAT(command, _DATA)* data)
#define EMIT_EVENT(destClass, dest, command, ...) auto* PPCAT(data, __LINE__) = new destClass::PPCAT(command, _DATA)({__VA_ARGS__}); (dest)->receive((void*)this, destClass::command, PPCAT(data, __LINE__))
#define PROCESS_EVENT(sender, command, data) PPCAT(command, _FUNC)(sender, (PPCAT(command, _DATA)*)(data))

namespace projectSolar::Events
{
	class EventHandler;
	
	struct Event
	{
		void* sender = nullptr;
		uint16_t command = 0;
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
	
	class EventHandler
	{
	public:
		enum : uint16_t
		{
			UNKNOWN_COMMAND = 0
		};
		
		explicit EventHandler(const size_t& threadsNumber);
		virtual ~EventHandler();

		void receive(void* sender, const uint16_t& command, void* data);

	protected:
		virtual void processEvent(const Event& ev) = 0;
	
	private:
		size_t m_threadsNumber;
		EventQueue m_events;
		std::mutex m_processing;

		void tryStartProcessing();
		void startProcessing();
		void process();
	};
}
