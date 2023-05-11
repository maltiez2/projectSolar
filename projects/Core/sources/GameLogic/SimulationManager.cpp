#include "SubscriptionManager.h"
#include "SimulationManager.h"
#include "SettingsManager.h"
#include "UniverseManager.h"
#include "ObjectManager.h"

namespace projectSolar::GameLogic
{
	using ObM = ObjectManager;
	
	SimulationManager::SimulationManager(std::shared_ptr<Simulation::SimulationRunner> simulationRunner, const size_t& threadsNumber) :
		EventHandler(threadsNumber),
		m_simulation(simulationRunner)
	{
		m_perorfmace = ObM::get().managerECS->registry.create();
		ObM::get().managerECS->registry.emplace<Components::SimulationPerformance>(m_perorfmace, 0.0f, 0ui16);
	}
	void SimulationManager::run()
	{
		auto performance = m_simulation->run(getRunParams());

		setPerformance(performance);

		EMIT_EVENT(SIM_DATA_UPDATE);
	}
	Simulation::SimulationRunner::Params SimulationManager::getRunParams()
	{
		return { 10.0f, 5e-2, 0.5f, 144, 10, -0.1f };
	}
	const Components::SimulationPerformance& SimulationManager::getPerformance() const
	{
		return ObM::get().managerECS->registry.get<Components::SimulationPerformance>(m_perorfmace);
	}

	void SimulationManager::setPerformance(const Simulation::SimulationRunner::Performance& performance)
	{
		ObM::get().managerECS->registry.patch<Components::SimulationPerformance>(m_perorfmace, [performance](Components::SimulationPerformance& settings)
			{
				settings.stepsPerSecond = performance.stepsPerSecond;
				settings.subStepsNumber = performance.subStepsNumber;
			}
		);
	}
}