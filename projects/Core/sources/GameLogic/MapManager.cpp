#include "MapManager.h"


namespace projectSolar::GameLogic
{
	void MapManager::processEvent(uint8_t eventType, uint8_t* data)
	{
		switch (eventType)
		{
			EVENTS_DEF_UNKNOWN();
			EVENT_DEF(SIMULATION_UPDATED);
			{
				//
			}
			EVENT_DEF(SET_CAMERA_AT);
			{
				m_layer->setCameraPosition(eventData.x, eventData.y, eventData.z);
			}
			EVENT_DEF(SET_CAMERA_ON);
			{
				switch (eventData.object)
				{
				case Objects::CENTRE:
					m_layer->setCameraPosition(0, 0, 0);
					break;
				case Objects::ATTRACTOR:
					m_layer->setCameraOn<Components::Attractor>(eventData.id);
					break;
				case Objects::ATTRACTANT:
					m_layer->setCameraOn<Components::Attractant>(eventData.id);
					break;
				case Objects::PROPULSED:
					m_layer->setCameraOn<Components::Propulsed>(eventData.id);
					break;
				default:
					LOG_ASSERT(false, "[MapManager] Unknown object type to follow");
				}
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
