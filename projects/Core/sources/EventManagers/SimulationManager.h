#pragma once

#include "EventHandler.h"
#include "Layers/SimulationLayer.h"
#include "Logger.h"

#include <memory>
#include <vector>


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

		EVENT_DECL(SET_SIM_STEP, 101, double stepSize);
		EVENT_DECL(SET_SIM_LOAD, 102, double timeRestrinctionSeconds);
		EVENT_DECL(ADD_TO_SIM_ORDER, 103, size_t simulation; size_t first; size_t last);
		EVENT_DECL(EXCLUDE_FROM_SIM_ORDER, 104, size_t simulation; size_t first; size_t last);
		EVENT_DECL(GENERATE_DEBUG_DATA, 105);
		EVENT_DECL(SAVE_DATA, 106, std::array<char, 128> saveName);
		EVENT_DECL(LOAD_DATA, 107, std::array<char, 128> saveName);
		EVENT_DECL(NEW_DATA_ADDED, 108, size_t totalDataSize);
		

	private:
		std::shared_ptr<Layers::SimLayer> m_layer;

		void processEvent(uint8_t eventType, uint8_t* data) override;
		void constructSimulations();
		
	};
}