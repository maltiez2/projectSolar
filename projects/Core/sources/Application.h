#pragma once

#include "Simulation.h"
#include "Graphics.h"

#include <memory>


namespace projectSolar
{
	class Application
	{
	public:
		Application(Simulation::SimulationRunner& simulation);
		~Application();

		void run();

	private:
		Simulation::SimulationRunner& m_simulation;
		Window* m_window;
	};
}