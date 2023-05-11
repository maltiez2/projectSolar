#pragma once

#include "ECS/EntityComponentSystem.h"
#include "EventHandler.h"
#include "Simulation.h"
#include "Logger.h"

#include <memory>


namespace projectSolar::GameLogic
{
	class SimulationManager : public EventHandler
	{
	public:
		SimulationManager(std::shared_ptr<Simulation::SimulationRunner> simulationRunner, const size_t& threadsNumber = 1);
		~SimulationManager() = default;

		Components::SimulationPerformance run();

		Simulation::SimulationRunner::Params getRunParams();

		const Components::SimulationPerformance& getPerformance() const;

	private:
		std::shared_ptr<Simulation::SimulationRunner> m_simulation;
		
		entt::entity m_perorfmace;

		void setPerformance(const Simulation::SimulationRunner::Performance& performance);
	};
}