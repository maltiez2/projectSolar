#pragma once

#include "DoubleBufferedQueue.h"

#include <queue>
#include <thread>
#include <vector>
#include <barrier>
#include <semaphore>

#define PPCAT_NX(A, B) A ## B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define SLOT_DECL(command, ...) struct PPCAT(command, _DATA) {__VA_ARGS__;}; void command(void* sender, PPCAT(command, _DATA)* data)
#define SLOT_IMPL(command, namespc) void namespc::command(void* sender, PPCAT(command, _DATA)* data)
#define SEND_EVENT(destClass, dest, command, ...) auto* PPCAT(data, __LINE__) = new destClass::PPCAT(command, _DATA)({__VA_ARGS__}); (dest)->receive((void*)this, (EventHandler::EventFunc)&destClass::command, PPCAT(data, __LINE__))
#define SEND_COMMAND(dest, command, data) (dest)->receive((void*)this, (EventHandler::EventFunc)command, data)


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
		
		explicit EventHandler(const size_t& threadsNumber);
		virtual ~EventHandler();

		void receive(void* sender, EventFunc command, void* data);
	
	private:
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
		
		DoubleBufferedQueue<Event> m_events;

		void startProcessing();
		void process();
	};
}
