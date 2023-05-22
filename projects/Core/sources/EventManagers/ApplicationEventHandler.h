#pragma once

#include <memory>

#include "EventHandler.h"

namespace projectSolar
{
	class Application;
	
	class ApplicationEventHandler : public Events::EventHandler
	{
	public:
		explicit ApplicationEventHandler(Application& app);
		~ApplicationEventHandler() override;

		EVENT_DECL(CLOSE_WINDOW, 101);
		EVENT_DECL(SET_RUN_SIMULATION, 102, bool run);
		EVENT_DECL(SET_SIM_LOAD, 103, float load);

	private:
		Application& m_app;

		void processEvent(uint8_t eventType, uint8_t* data) override;
	};
}


