#include "GuiManager.h"

#include "MapManager.h"
#include "Application.h"
#include "Windows/Windows.h"

namespace projectSolar::GameLogic
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
			}
			EVENT_DEF(GUI_UPDATED);
			{
				float scale = m_windows->get<Windows::Debug>("debug")->scale * 0.05f;
				SEND_EVENT(SET_CAMERA_SCALE, MapManager, Com::get().Map, scale);

				bool runSimulation = m_windows->get<Windows::Debug>("debug")->runSimulation;
				SEND_EVENT(SET_RUN_SIMULATION, ApplicationEventHandler, Com::get().Application, runSimulation);

				updateWidnowsInteraction();
			}
			EVENTS_DEF_DEFAULT();
			break;
		}
	}

	void GuiManager::setUpGUI()
	{
		m_windows->add<Windows::Debug>("debug", true);
		m_windows->add<Graphics::DemoWindow>("demo", false);
		m_windows->add<Windows::PropulsionControl>("prop", true);
	}

	void GuiManager::updateWidnowsInteraction()
	{
		if (m_windows->get<Windows::Debug>("debug")->showDemoWindow)
		{
			m_windows->show("demo", true);
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
