#include "GuiManager.h"

#include "MapManager.h"
#include "Application.h"

namespace projectSolar::GameLogic
{
	void GuiManager::processEvent(uint8_t eventType, uint8_t* data)
	{
		switch (eventType)
		{
			EVENTS_DEF_UNKNOWN();
			EVENT_DEF(SIMULATION_UPDATED);
			{
				if (m_windows->get<Graphics::PropulsionControlWindow>("prop")->followPlayer)
				{
					SEND_EVENT(SET_CAMERA_ON, MapManager, Com::get().Map, MapManager::Objects::PROPULSED, 0);
				}
			}
			EVENT_DEF(GUI_UPDATED);
			{
				float scale = (float)m_windows->get<Graphics::DebugWindow>("debug")->scale * 0.05f;
				SEND_EVENT(SET_CAMERA_SCALE, MapManager, Com::get().Map, scale);

				bool runSimulation = m_windows->get<Graphics::DebugWindow>("debug")->runSimulation;
				SEND_EVENT(SET_RUN_SIMULATION, ApplicationEventHandler, Com::get().Application, runSimulation);
			}
			EVENTS_DEF_DEFAULT();
			break;
		}
	}

	void GuiManager::setUpGUI()
	{
		m_windows->add<Graphics::NotificationWindow>("test", false, "Test window", "Test text of test window");
		m_windows->add<Graphics::DebugWindow>("debug", true);
		m_windows->add<Graphics::DemoWindow>("demo", false);
		m_windows->add<Graphics::PropulsionControlWindow>("prop", true);
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
