#pragma once

#include "ECS/EntityComponentSystem.h"
#include "EventHandler.h"
#include "Simulation.h"
#include "Logger.h"


namespace projectSolar::GameLogic
{
	class SimulationManager : public EventHandler
	{
	public:
		SimulationManager(Simulation::SimulationRunner& simulationRunner, ECS::EntityManager& entityManager, const size_t& threadsNumber = 1);
		~SimulationManager() = default;

		void run();

		entt::entity getPerformance();

	private:
		ECS::EntityManager& m_entityManager;
		Simulation::SimulationRunner& m_simulationRunner;
		entt::entity m_perorfmace;

		void setPerformance(const Simulation::SimulationRunner::Performance& performance);
	};
}