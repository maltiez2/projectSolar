#include "pch.h"

#include "MapManager.h"


namespace projectSolar::EventManagers
{
	void MapManager::processEvent(uint8_t eventType, uint8_t* data)
	{
		switch (eventType)
		{
			EVENTS_DEF_UNKNOWN();
			EVENT_DEF(SIMULATION_UPDATED);
			{
				// Unused currently
			}
			EVENT_DEF(SET_CAMERA_AT);
			{
				m_layer->setCameraPosition(eventData.x, eventData.y, eventData.z);
			}
			EVENT_DEF(SET_CAMERA_ON);
			{
				m_layer->setCameraOn(eventData.id);
			}
			EVENT_DEF(RESET_CAMERA);
			{
				m_layer->setCameraPosition(0, 0, 0);
			}
			EVENT_DEF(SET_CAMERA_SCALE);
			{
				m_layer->setCameraScale(eventData.scale);
			}
			EVENTS_DEF_DEFAULT();
			break;
		}
	}

	MapManager::MapManager(std::shared_ptr<Layers::MapLayer> layer, const size_t& threadsNumber) :
		EventHandler(threadsNumber),
		m_layer(layer)
	{
		
	}
	MapManager::~MapManager()
	{
		destroyWorkers();
	}
}
