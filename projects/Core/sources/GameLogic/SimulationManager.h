#pragma once

#include "EventHandler.h"
#include "Layers/SimulationLayer.h"
#include "Logger.h"

#include <memory>
#include <vector>


namespace projectSolar::GameLogic
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

		EVENT_DECL(SET_SIM_STEP, 11, double stepSize);
		EVENT_DECL(SET_SIM_LOAD, 12, double timeRestrinctionSeconds);
		EVENT_DECL(NEW_DATA_ADDED, 19, size_t totalDataSize);

	private:
		std::shared_ptr<Layers::SimLayer> m_layer;

		void processEvent(uint8_t eventType, uint8_t* data) override;
		void constructSimulations();
		
	};
}