#pragma once

#include "ECS/EntityComponentSystem.h"
#include "EventHandler.h"
#include "Simulation.h"
#include "Logger.h"

#include <memory>


namespace projectSolar::GameLogic
{
	class SimulationManager : public Events::EventHandler
	{
	public:
		SimulationManager(std::shared_ptr<Simulation::SimulationRunner> simulationRunner, const size_t& threadsNumber = 1);
		~SimulationManager() override;

		Components::SimulationPerformance run();
		const Components::SimulationPerformance& getPerformance() const;
		Simulation::SimulationRunner::Params getRunParams() const;

	private:
		void processEvent(uint8_t eventType, uint8_t* data) override;
		
		std::shared_ptr<Simulation::SimulationRunner> m_simulation;
		entt::entity m_perorfmace;	
		void setPerformance(const Simulation::SimulationRunner::Performance& performance);
	};
}