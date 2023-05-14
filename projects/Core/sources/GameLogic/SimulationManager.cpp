#include "SimulationManager.h"
#include "CommunicationManager.h"

namespace projectSolar::GameLogic
{
	
	SimulationManager::SimulationManager(std::shared_ptr<Layers::SimLayer> layer, const size_t& threadsNumber) :
		EventHandler(threadsNumber),
		m_layer(layer)
	{
		constructSimulations();
	}
	SimulationManager::~SimulationManager()
	{
		destroyWorkers();
	}

	void SimulationManager::processEvent(uint8_t eventType, uint8_t* data)
	{
		switch (eventType)
		{
			EVENTS_DEF_UNKNOWN();
			EVENT_DEF(SET_SIM_STEP);
			{
				m_layer->setStepSize(eventData.stepSize);
			}
			EVENT_DEF(SET_SIM_LOAD);
			{
				m_layer->setTimeRest(eventData.timeRestrinctionSeconds);
			}
			EVENT_DEF(NEW_DATA_ADDED);
			{
				m_layer->setSimOrder(MOTION_SIM, { {0, eventData.totalDataSize - 1} });
				m_layer->setSimOrder(GRAVITY_SIM, { {0, eventData.totalDataSize - 1} });
			}
			EVENTS_DEF_DEFAULT();
				break;
		}
	}

	void SimulationManager::constructSimulations()
	{
		auto motionSim = m_layer->add<Simulation::Motion>(MOTION_SIM, Simulation::Motion::Params());
		auto gravSim = m_layer->add<Simulation::Gravity>(GRAVITY_SIM, Simulation::Gravity::Params(), &motionSim->data);

		m_layer->setSimOrder(MOTION_SIM, { {} });
		m_layer->setSimOrder(GRAVITY_SIM, { {} });
	}

	std::vector<Simulation::Motion::Data>& SimulationManager::getMotionData()
	{
		return m_layer->get<Simulation::Motion>(MOTION_SIM)->data.getData();
	}
}