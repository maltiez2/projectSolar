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
			EVENT_DEF(SIMULATION_UPDATED); //****************************************************************************************************
			{
				/*if (m_windows->get<Windows::Debug>(DEBUG)->followPlayer)
				{
					auto view = Com::get().ECS->getView<Components::Player>();

					for (auto entity : view)
					{
						size_t motionDataIndex = Com::get().ECS->get<Components::Dynamic>(entity).motionDataIndex;
						SEND_EVENT(SET_CAMERA_ON, MapManager, Com::get().Map, motionDataIndex);
						break;
					}
				}
				else
				{
					SEND_EVENT(RESET_CAMERA, MapManager, Com::get().Map);
				}*/

				m_windows->get<Windows::Debug>(DEBUG)->stepsPerFrame = eventData.stepsPerFrame;
				m_windows->get<Windows::Debug>(DEBUG)->secondsPerStep = eventData.secondsPerStep;
			}
			EVENT_DEF(GUI_UPDATED);//****************************************************************************************************
			{
				float scale = m_windows->get<Windows::Debug>(DEBUG)->scale * 0.05f;
				SEND_EVENT(SET_CAMERA_SCALE, MapManager, Com::get().Map, scale);

				bool runSimulation = m_windows->get<Windows::Debug>(DEBUG)->runSimulation;
				SEND_EVENT(SET_RUN_SIMULATION, ApplicationEventHandler, Com::get().Application, runSimulation);

				updateWidnowsInteraction();
			}
			EVENT_DEF(MAP_OBJECTS_UNDER_CURSOR); //****************************************************************************************************
			{
				m_windows->get<Windows::Debug>(DEBUG)->objUnderCursor.clear();
				
				for (entt::entity& entity : eventData.objects)
				{
					if (entity == entt::null)
					{
						continue;
					}

					if (Com::get().ECS->has<Components::CelestialObject>(entity))
					{
						auto& celestObj = Com::get().ECS->get<Components::CelestialObject>(entity);
						m_windows->get<Windows::Debug>(DEBUG)->objUnderCursor.emplace_back(celestObj.name.data());
					}
					else
					{
						auto& mapObj = Com::get().ECS->get<Components::MapObject>(entity);
						m_windows->get<Windows::Debug>(DEBUG)->objUnderCursor.emplace_back(std::to_string(mapObj.id));
					}
				}
			}
			EVENTS_DEF_DEFAULT(); //****************************************************************************************************
			break;
		}
	}

	void GuiManager::setUpGUI()
	{
		m_windows->add<Windows::Debug>(DEBUG, true);
		m_windows->add<Windows::Gravity>(GRAVITY, true);
		m_windows->add<Graphics::DemoWindow>(DEMO, false);
	}

	void GuiManager::updateWidnowsInteraction()
	{
		if (m_windows->get<Windows::Debug>(DEBUG)->showDemoWindow)
		{
			m_windows->show(DEMO, true);
		}

		if (m_windows->get<Windows::Debug>(DEBUG)->generateDebugData)
		{
			size_t primary = m_windows->get<Windows::Debug>(DEBUG)->primaryObj;
			size_t secondary = m_windows->get<Windows::Debug>(DEBUG)->secondaryObj;
			size_t ternary = m_windows->get<Windows::Debug>(DEBUG)->ternaryObj;
			SEND_EVENT(GENERATE_DEBUG_DATA, SimulationManager, Com::get().simulation, primary, secondary, ternary);
		}

		if (m_windows->get<Windows::Debug>(DEBUG)->saveData)
		{
			SEND_EVENT(SAVE, SavesManager, Com::get().Saves, Components::LongTitle{ "debug_0" });
		}

		if (m_windows->get<Windows::Debug>(DEBUG)->loadData)
		{
			SEND_EVENT(LOAD, SavesManager, Com::get().Saves, Components::LongTitle{ "debug_0" });
		}

		auto& prevLoad = m_windows->get<Windows::Debug>(DEBUG)->prevSimLoad;
		const auto& currentLoad = m_windows->get<Windows::Debug>(DEBUG)->simLoad;
		if (currentLoad != prevLoad)
		{
			prevLoad = currentLoad;
			SEND_EVENT(SET_SIM_LOAD, ApplicationEventHandler, Com::get().Application, currentLoad);
		}

		auto& prevRate = m_windows->get<Windows::Debug>(DEBUG)->prevSimRate;
		const auto& currentRate = m_windows->get<Windows::Debug>(DEBUG)->simRate;
		if (currentRate != prevRate)
		{
			prevRate = currentRate;
			SEND_EVENT(SET_SIM_RATE, SimulationManager, Com::get().simulation, currentRate * 1e-5);
		}

		auto& prevGranularity = m_windows->get<Windows::Gravity>(GRAVITY)->prevGranularity;
		auto& prevMinTaskSize = m_windows->get<Windows::Gravity>(GRAVITY)->prevMinTaskSize;
		const auto& currentGranularity = m_windows->get<Windows::Gravity>(GRAVITY)->granularity;
		const auto& currentMinTaskSize = m_windows->get<Windows::Gravity>(GRAVITY)->minTaskSize;
		if (prevGranularity != currentGranularity || prevMinTaskSize != currentMinTaskSize)
		{
			prevGranularity = currentGranularity;
			prevMinTaskSize = currentMinTaskSize;
			SEND_EVENT(SET_SIM_RUN_PARAMS, SimulationManager, Com::get().simulation, SimulationManager::PRIMARY_ATTRACTORS_GRAVITY, (size_t)currentGranularity, (size_t)currentMinTaskSize);
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
