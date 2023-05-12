#include "SimulationManager.h"
#include "CommunicationManager.h"

namespace projectSolar::GameLogic
{
	
	SimulationManager::SimulationManager(std::shared_ptr<Simulation::SimulationRunner> simulationRunner, const size_t& threadsNumber) :
		EventHandler(threadsNumber),
		m_simulation(simulationRunner)
	{
	}
	SimulationManager::~SimulationManager()
	{
		destroyWorkers();
	}

	Components::SimulationPerformance SimulationManager::run()
	{
		auto performance = m_simulation->run(getRunParams());

		setPerformance(performance);

		EMIT_EVENT(SIMULATION_UPDATED, performance.secondsPerStep, performance.subStepsNumber);

		return { performance.secondsPerStep, performance.subStepsNumber };
	}
	Simulation::SimulationRunner::Params SimulationManager::getRunParams() const
	{
		return { 10.0f, 5e-2, 0.5f, 144, 10, -0.1f };
	}
	const Components::SimulationPerformance& SimulationManager::getPerformance() const
	{
		return Com::get().ECS->registry.get<Components::SimulationPerformance>(m_perorfmace);
	}

	void SimulationManager::setPerformance(const Simulation::SimulationRunner::Performance& performance)
	{
		/*Com::get().ECS->registry.patch<Components::SimulationPerformance>(m_perorfmace, [performance](Components::SimulationPerformance& settings)
			{
				settings.secondsPerStep = performance.secondsPerStep;
				settings.subStepsNumber = performance.subStepsNumber;
			}
		);*/
	}

	void SimulationManager::processEvent(uint8_t eventType, uint8_t* data)
	{
		switch (eventType)
		{
			EVENTS_DEF_UNKNOWN()
			EVENTS_DEF_DEFAULT()
				break;
		}
	}
}