#pragma once

#include "EntityStack.h"

#include <map>
#include <string>
#include <shared_mutex>


namespace projectSolar::Simulation
{
	class Simulation;
	class SimulationRunner;
	class StepsDivider;

	class SimulationStack : public DataStructures::EntityStack<Simulation>
	{
	public:
		explicit SimulationStack(std::shared_ptr<SimulationRunner> runner);
		~SimulationStack() = default;
		
		void step();

	protected:		
		std::shared_ptr<SimulationRunner> m_runner;

		const uint16_t c_maxStep = 1;
		uint16_t m_step = 0;
	};
}