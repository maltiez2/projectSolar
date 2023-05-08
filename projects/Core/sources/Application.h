#pragma once

#include "Simulation.h"
#include "Layers.h"
#include "Graphics.h"

#include "Events/EventHandler.h"

#include <memory>

namespace projectSolar
{
	class Application : public Events::EventHandler
	{
	public:
		
		
		Application(Simulation::SimulationRunner& simulation, const WindowProperties& windowProps = WindowProperties());
		~Application();

		void run();

		Simulation::SimulationRunner& m_simulation;
		LayersManager m_layers;
		Window* m_window;

	protected:
		void processEvent(const Events::Event& ev) override;

	private:
		bool m_running = true;

		void processInputEvents();

	public: // SLOTS

		SLOT(CLOSE_WINDOW)
		{
			LOG_DEBUG("CLOSE_WINDOW");
		}

		SLOT(DEBUG_MESSAGE, const char* message)
		{
			LOG_DEBUG("DEBUG_MESSAGE: ", data->message);
		}
	};
}