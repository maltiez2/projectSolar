#pragma once

#include "Simulation.h"
#include "Layers.h"
#include "Graphics.h"

#include <memory>

namespace projectSolar
{
	class Application
	{
	public:
		Application(Simulation::SimulationRunner& simulation, const WindowProperties& windowProps = WindowProperties());
		~Application();

		void run();

	
		Simulation::SimulationRunner& m_simulation;
		LayersManager m_layers;
		Window* m_window;

	private:
		bool m_running = true;

		void processEvents();
	};
}