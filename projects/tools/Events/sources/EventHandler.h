#pragma once

#include "DoubleBufferedQueue.h"

#include <queue>
#include <thread>
#include <vector>
#include <barrier>
#include <semaphore>

// @TODO From Darian
//Personally, I use type erased lambda wrapper functions that accept void payloads,
//I force clean - up to occur inside of them, 
//require trivially relocatable payloads and lambda captures,
//and allocate all payloads in a ring buffer
//
//+ [](void* body, void* payload) {
//	(*static_cast<Lambda*>(body))(*static_cast<Payload*>(payload));
//}
//
// the body is tightly allocated with other handlers, and the payload is linearly allocated into a ring buffer

#define PPCAT_NX(A, B) A ## B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define SLOT_DECL(command, ...) struct PPCAT(command, _DATA) {__VA_ARGS__;}; void command(PPCAT(command, _DATA)* data)
#define SLOT_IMPL(command, namespc) void namespc::command(PPCAT(command, _DATA)* data)
#define SEND_COMMAND(dest, command, inputData) (dest)->receive(command, data)
#define SEND_EVENT(destClass, dest, command, ...) \
	auto* PPCAT(data, __LINE__) = new destClass::PPCAT(command, _DATA)({__VA_ARGS__});\
	(dest)->receive(\
	[](void* receiver, void* data)\
	{\
		((destClass*)receiver)->command((destClass::PPCAT(command, _DATA)*)data);\
		delete((destClass::PPCAT(command, _DATA)*)data);\
	},\
	PPCAT(data, __LINE__))


namespace projectSolar
{
	class EventHandler
	{
	public:
		using EventFunc = void(*)(void* receiver, void* data);

		struct Event
		{
			EventHandler::EventFunc command = nullptr;
			void* data = nullptr;
		};
		
		explicit EventHandler(const size_t& threadsNumber);
		virtual ~EventHandler();

		void receive(EventFunc command, void* data);
	
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
