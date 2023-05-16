#include "pch.h"

#include "SimulationManager.h"
#include "CommunicationManager.h"
#include "ECS/EntityComponentSystem.h"

namespace projectSolar::EventManagers
{
	
	SimulationManager::SimulationManager(std::shared_ptr<Layers::SimLayer> layer, const size_t& threadsNumber) :
		EventHandler(threadsNumber),
		m_layer(layer)
	{
		m_simulationEntity = Com::get().ECS->create();
		Com::get().ECS->insert<Components::SimSettings>(m_simulationEntity, 1.0, 1.0);
		Com::get().ECS->insert<Components::SimPerformance>(m_simulationEntity, 1ui64, 0.0f);
		
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
			EVENT_DEF(ADD_TO_SIM_ORDER);
			{
				m_layer->addToSimOrder(eventData.simulation, { eventData.first, eventData.last });
			}
			EVENT_DEF(EXCLUDE_FROM_SIM_ORDER);
			{
				m_layer->excludeFromSimOrder(eventData.simulation, { eventData.first, eventData.last });
			}
			EVENT_DEF(GENERATE_DEBUG_DATA);
			{
				m_layer->generateDebugLayout(MOTION_SIM, GRAVITY_SIM);
			}
			EVENT_DEF(SIMULATION_UPDATED);
			{
				Com::get().ECS->replace<Components::SimPerformance>(m_simulationEntity, eventData.stepsPerFrame, eventData.secondsPerStep);
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