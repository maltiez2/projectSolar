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
			EVENT_DEF(SET_SIM_RATE);
			{
				m_layer->setTimePerSecond(eventData.timePerSecond);
			}
			EVENT_DEF(SET_SIM_LOAD);
			{
				m_layer->setTimeRest(eventData.timeRestrinctionSeconds);
			}
			EVENT_DEF(GENERATE_DEBUG_DATA);
			{
				m_layer->generateDebugLayout(MOTION_SIM, GRAVITY_SIM);
			}
			EVENT_DEF(SET_DEBUG_DATA_OBJ_NUMBER);
			{
				m_layer->setObjNumberInDebugLayout(eventData.number);
			}
			EVENT_DEF(SIMULATION_UPDATED);
			{
				if (Com::get().ECS->exists(m_simulationEntity))
				{
					Com::get().ECS->replace<Components::SimPerformance>(m_simulationEntity, eventData.stepsPerFrame, eventData.secondsPerStep);
				}
			}
			EVENT_DEF(OBJ_DRUGGED);
			{
				LOG_DEBUG("Obj drugged at: ", eventData.newX, " : ", eventData.newY, " : ", eventData.newZ);

				auto& motionData = Com::get().ECS->get<Components::Dynamic>(eventData.objects[0]);
				auto& simData = m_layer->get<Simulation::Motion>(MOTION_SIM)->data.getData();
				auto& simBuffer = m_layer->get<Simulation::Motion>(MOTION_SIM)->data.getBuffer();
				
				std::unique_lock wirteLock(m_layer->m_dataMutex);
				simData[motionData.motionDataIndex].position = { eventData.newX, eventData.newY, eventData.newZ };
				simData[motionData.motionDataIndex].velocity = { 0.0, 0.0, 0.0 };
				simBuffer[motionData.motionDataIndex].position = { eventData.newX, eventData.newY, eventData.newZ };
				simBuffer[motionData.motionDataIndex].velocity = { 0.0, 0.0, 0.0 };
				break;
			}
			EVENT_DEF(SET_SIM_RUN_PARAMS);
			{
				LOG_ASSERT(m_layer->ifAttached(eventData.simulation), "Simulation ", eventData.simulation, " not attached");
				m_layer->get<Simulation::Simulation>(eventData.simulation)->setRunParams({ eventData.granularity, eventData.minTaskSize });
			}
			EVENTS_DEF_DEFAULT();
				break;
		}
	}

	void SimulationManager::constructSimulations()
	{
		auto motionSim = m_layer->add<Simulation::Motion>(MOTION_SIM, Simulation::Motion::Params(m_layer->stepSize, m_layer->stepSize), Simulation::RunParams(1, 1));
		auto gravSim = m_layer->add<Simulation::Gravity>(GRAVITY_SIM, Simulation::Gravity::Params(), Simulation::RunParams(3, 50), &motionSim->data);

		std::vector<std::vector<Simulation::Task>> emptyOrder = { {} };
	}

	std::vector<Simulation::Motion::Data>& SimulationManager::getMotionData()
	{
		return m_layer->get<Simulation::Motion>(MOTION_SIM)->data.getData();
	}
}