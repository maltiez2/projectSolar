#include "pch.h"

#include "SimulationStack.h"
#include "SimulationRunner.h"
#include "Simulations/Simulation.h"
#include "Serializer.h"

namespace projectSolar::Simulation
{
	SimulationStack::SimulationStack(std::shared_ptr<SimulationRunner> runner) :
		m_runner(runner)
	{

	}

	void SimulationStack::step()
	{
		PROFILE_FUNCTION();

		std::vector<std::shared_ptr<Simulation>> group = {};

		uint8_t currentGroup = 0;
		
		for (const auto& [id, simulation] : m_attached)
		{
			if (simulation->skip(m_step))
			{
				continue;
			}

			if (currentGroup != simulation->getGroup())
			{
				runGroup(group);
				group.clear();
			}

			group.push_back(simulation);
		}

		runGroup(group);

		m_step = (m_step + 1) % (c_maxStep + 1);
	}

	void SimulationStack::runGroup(std::vector<std::shared_ptr<Simulation>>& group)
	{
		m_runner->run(group);

		for (const auto& ranSimulation : group)
		{
			ranSimulation->swapData();
		}
	}
}