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
		EVENT_DECL(ADD_TO_SIM_ORDER, 103, size_t simulation; size_t first; size_t last);
		EVENT_DECL(EXCLUDE_FROM_SIM_ORDER, 104, size_t simulation; size_t first; size_t last);
		EVENT_DECL(GENERATE_DEBUG_DATA, 105);
		EVENT_DECL(OBJ_DRUGGED, 106, float newX; float newY; float newZ; std::array<entt::entity, maxObjDragged> objects);
		

	private:
		std::shared_ptr<Layers::SimLayer> m_layer;
		entt::entity m_simulationEntity;

		void processEvent(uint8_t eventType, uint8_t* data) override;
		void constructSimulations();
	};
}