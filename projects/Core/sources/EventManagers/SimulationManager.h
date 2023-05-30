#pragma once

#include "EventHandler.h"
#include "Layers/SimulationLayer.h"
#include "Logger.h"

#include <memory>
#include <vector>
#include <EnTT/entt.hpp>


namespace projectSolar::EventManagers
{
	class SimulationManager : public Events::EventHandler
	{
	public:
		enum : size_t
		{
			GRAVITY_SIM,
			MOTION_SIM
		};
		
		SimulationManager(std::shared_ptr<Layers::SimLayer> layer, const size_t& threadsNumber = 1);
		~SimulationManager() override;

		std::vector<Simulation::Motion::Data>& getMotionData();

		static const size_t maxObjDragged = 8;

		EVENT_DECL(SIMULATION_UPDATED, 1, float secondsPerStep; size_t stepsPerFrame);

		EVENT_DECL(SET_SIM_RATE, 101, double timePerSecond);
		EVENT_DECL(SET_SIM_LOAD, 102, double timeRestrinctionSeconds);
		EVENT_DECL(GENERATE_DEBUG_DATA, 103);
		EVENT_DECL(SET_DEBUG_DATA_OBJ_NUMBER, 104, uint64_t number);
		EVENT_DECL(OBJ_DRUGGED, 105, float newX; float newY; float newZ; std::array<entt::entity, maxObjDragged> objects);
		EVENT_DECL(SET_SIM_RUN_PARAMS, 106, size_t simulation; size_t granularity; size_t minTaskSize);

	private:
		std::shared_ptr<Layers::SimLayer> m_layer;
		entt::entity m_simulationEntity;

		void processEvent(uint8_t eventType, uint8_t* data) override;
		void constructSimulations();
	};
}