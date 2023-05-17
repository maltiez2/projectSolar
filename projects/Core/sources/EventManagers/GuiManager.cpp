#include "pch.h"

#include "GuiManager.h"

#include "MapManager.h"
#include "Application.h"
#include "Windows/Windows.h"
#include "EventManagers/ApplicationEventHandler.h"
#include "EventManagers/SavesManager.h"
#include "ECS/Components.h"

namespace projectSolar::EventManagers
{
	void GuiManager::processEvent(uint8_t eventType, uint8_t* data)
	{
		switch (eventType)
		{
			EVENTS_DEF_UNKNOWN();
			EVENT_DEF(SIMULATION_UPDATED);
			{
				if (m_windows->get<Windows::Debug>("debug")->followPlayer)
				{
					SEND_EVENT(SET_CAMERA_ON, MapManager, Com::get().Map, 1);
				}
				else
				{
					SEND_EVENT(RESET_CAMERA, MapManager, Com::get().Map);
				}

				m_windows->get<Windows::Debug>("debug")->stepsPerFrame = eventData.stepsPerFrame;
				m_windows->get<Windows::Debug>("debug")->secondsPerStep = eventData.secondsPerStep;
			}
			EVENT_DEF(GUI_UPDATED);
			{
				float scale = m_windows->get<Windows::Debug>("debug")->scale * 0.05f;
				SEND_EVENT(SET_CAMERA_SCALE, MapManager, Com::get().Map, scale);

				bool runSimulation = m_windows->get<Windows::Debug>("debug")->runSimulation;
				SEND_EVENT(SET_RUN_SIMULATION, ApplicationEventHandler, Com::get().Application, runSimulation);

				updateWidnowsInteraction();
			}
			EVENT_DEF(MAP_OBJECTS_UNDER_CURSOR);
			{
				m_windows->get<Windows::Debug>("debug")->objUnderCursor.clear();
				
				for (entt::entity& entity : eventData.objects)
				{
					if (entity == entt::null)
					{
						continue;
					}

					if (Com::get().ECS->has<Components::CelestialObject>(entity))
					{
						auto& celestObj = Com::get().ECS->get<Components::CelestialObject>(entity);
						m_windows->get<Windows::Debug>("debug")->objUnderCursor.push_back(celestObj.name.data());
					}
					else
					{
						auto& mapObj = Com::get().ECS->get<Components::MapObject>(entity);
						m_windows->get<Windows::Debug>("debug")->objUnderCursor.push_back(std::to_string(mapObj.id));
					}
				}
			}
			EVENTS_DEF_DEFAULT();
			break;
		}
	}

	void GuiManager::setUpGUI()
	{
		m_windows->add<Windows::Debug>("debug", true);
		m_windows->add<Graphics::DemoWindow>("demo", false);
	}

	void GuiManager::updateWidnowsInteraction()
	{
		if (m_windows->get<Windows::Debug>("debug")->showDemoWindow)
		{
			m_windows->show("demo", true);
		}

		if (m_windows->get<Windows::Debug>("debug")->generateDebugData)
		{
			SEND_EVENT(GENERATE_DEBUG_DATA, EventManagers::SimulationManager, Com::get().simulation);
		}

		if (m_windows->get<Windows::Debug>("debug")->saveData)
		{
			SEND_EVENT(SAVE, EventManagers::SavesManager, Com::get().Saves, Components::LongTitle{ "debug_0" });
		}

		if (m_windows->get<Windows::Debug>("debug")->loadData)
		{
			SEND_EVENT(LOAD, EventManagers::SavesManager, Com::get().Saves, Components::LongTitle{ "debug_0" });
		}
	}
	
	GuiManager::GuiManager(std::shared_ptr<Layers::GuiLayer> layer, const size_t& threadsNumber) :
		EventHandler(threadsNumber),
		m_layer(layer),
		m_windows(layer->getWindowsManager())
	{
		setUpGUI();
	}
	GuiManager::~GuiManager()
	{
		destroyWorkers();
	}

	std::shared_ptr<Graphics::GuiWindowsManager> GuiManager::getWidnows()
	{
		return m_windows;
	}
}
