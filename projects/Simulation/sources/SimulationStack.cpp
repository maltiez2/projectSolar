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
		
		for (auto& [id, simulation] : m_attached)
		{
			if (!simulation->skip(m_step))
			{
				group.push_back(simulation);
				//m_runner->run({ simulation });
			}
		}

		m_runner->run(group);

		for (const auto& [id, simulation] : m_attached)
		{
			if (!simulation->skip(m_step))
			{
				simulation->swapData();
			}
		}

		m_step = (m_step + 1) % (c_maxStep + 1);
	}
}