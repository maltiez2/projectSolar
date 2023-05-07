#pragma once

#include "Simulation.h"

namespace projectSolar
{
	class Application
	{
	public:
		Application(Simulation::SimulationRunner& simulation);
		~Application() = default;

		void run();

	private:
		Simulation::SimulationRunner& m_simulation;
	};
}