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

		void onEvent(Event& ev);

	private:
		bool onWindowClose(WindowCloseEvent& ev);

		Simulation::SimulationRunner& m_simulation;
		LayersManager m_layers;
		Window* m_window;
		bool m_running = true;
	};
}