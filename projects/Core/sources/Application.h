#pragma once

#include "Simulation.h"
#include "Layers.h"
#include "Graphics.h"

#include "EventHandler.h"

#include <memory>

namespace projectSolar
{
	class Application : public EventHandler
	{
	public:
		
		
		Application(Simulation::SimulationRunner& simulation, const Graphics::WindowProperties& windowProps = Graphics::WindowProperties());
		~Application();

		void run();

		Simulation::SimulationRunner& m_simulation;
		LayersManager m_layers;
		Graphics::Window* m_window;

	private:
		bool m_running = true;

		void processInputEvents();

	public: // SLOTS

		SLOT_DECL(CLOSE_WINDOW)
		{
			LOG_DEBUG("CLOSE_WINDOW");
		}

		SLOT_DECL(DEBUG_MESSAGE, const char* message)
		{
			LOG_DEBUG("DEBUG_MESSAGE: ", data->message);
		}
	};
}