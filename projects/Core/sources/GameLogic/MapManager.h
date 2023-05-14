#pragma once

#include "ECS/EntityComponentSystem.h"
#include "EventHandler.h"
#include "Layers/MapLayer.h"
#include "Logger.h"

#include <memory>


namespace projectSolar::GameLogic
{
	class MapManager : public Events::EventHandler
	{
	public:
		MapManager(std::shared_ptr<Layers::MapLayer> layer, const size_t& threadsNumber = 1);
		~MapManager() override;

		EVENT_DECL(SIMULATION_UPDATED, 1, float secondsPerStep; uint16_t subSteps);
		EVENT_DECL(SET_CAMERA_AT, 41, float x; float y; float z);
		EVENT_DECL(SET_CAMERA_ON, 42, size_t id);
		EVENT_DECL(RESET_CAMERA, 43);
		EVENT_DECL(SET_CAMERA_SCALE, 44, float scale);

	private:
		void processEvent(uint8_t eventType, uint8_t* data) override;

		std::shared_ptr<Layers::MapLayer> m_layer;
	};
}