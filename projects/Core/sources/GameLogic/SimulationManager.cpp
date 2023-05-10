#include "SubscriptionManager.h"
#include "SimulationManager.h"
#include "SettingsManager.h"
#include "UniverseManager.h"
#include "ObjectManager.h"

namespace projectSolar::GameLogic
{
	SimulationManager::SimulationManager(Simulation::SimulationRunner& simulationRunner, ECS::EntityManager& entityManager, const size_t& threadsNumber) :
		EventHandler(threadsNumber),
		m_entityManager(entityManager),
		m_simulationRunner(simulationRunner)
	{
		m_perorfmace = m_entityManager.registry.create();
		m_entityManager.registry.emplace<Components::SimulationPerformance>(m_perorfmace, 0.0, 0);
	}
	void SimulationManager::run()
	{
		auto* settingsManager = ObjectManager::get<SettingsManager>(ObjectManager::SETTINGS_MANAGER);
		Components::GameSettings gameSettings = m_entityManager.registry.get<Components::GameSettings>(settingsManager->getGameSettings());
		Components::VideoSettings videoSettings = m_entityManager.registry.get<Components::VideoSettings>(settingsManager->getVideoSettings());
		auto* universeManager = ObjectManager::get<UniverseManager>(ObjectManager::UNIVERSE_MANAGER);
		Components::Universe universeParams = m_entityManager.registry.get<Components::Universe>(universeManager->getUniverse());
		
		auto performance = m_simulationRunner.run(
			{
				universeParams.gravitationalConstant,
				universeParams.gravitationalConstant,
				gameSettings.simulationLoadFactor,
				videoSettings.targetFPS,
				gameSettings.simulationInitialStepsNumber,
				gameSettings.simulationStepsAdjustBias
			}
		);

		setPerformance(performance);

		EMIT_EVENT(SIM_DATA_UPDATE);
	}
	entt::entity SimulationManager::getPerformance()
	{
		return m_perorfmace;
	}
	void SimulationManager::setPerformance(const Simulation::SimulationRunner::Performance& performance)
	{
		m_entityManager.registry.patch<Components::SimulationPerformance>(m_perorfmace, [performance](Components::SimulationPerformance& settings)
			{
				settings.stepsPerSecond = performance.stepsPerSecond;
				settings.subStepsNumber = performance.subStepsNumber;
			}
		);
	}
}