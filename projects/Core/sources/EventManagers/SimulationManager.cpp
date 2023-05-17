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
				if (Com::get().ECS->exists(m_simulationEntity))
				{
					Com::get().ECS->replace<Components::SimPerformance>(m_simulationEntity, eventData.stepsPerFrame, eventData.secondsPerStep);
				}
			}
			EVENT_DEF(OBJ_DRUGGED);
			{
				LOG_DEBUG("Obj drugged at: ", eventData.newX, " : ", eventData.newY, " : ", eventData.newZ);

				auto& motionData = Com::get().ECS->get<Components::Dynamic>(eventData.objects[0]);
				auto& data = m_layer->get<Simulation::Motion>(MOTION_SIM)->data.getData();
				auto& buffer = m_layer->get<Simulation::Motion>(MOTION_SIM)->data.getBuffer();
				
				std::unique_lock wirteLock(m_layer->m_dataMutex);
				data[motionData.motionDataIndex].position = { eventData.newX, eventData.newY, eventData.newZ };
				data[motionData.motionDataIndex].velocity = { 0.0, 0.0, 0.0 };
				buffer[motionData.motionDataIndex].position = { eventData.newX, eventData.newY, eventData.newZ };
				buffer[motionData.motionDataIndex].velocity = { 0.0, 0.0, 0.0 };
				break;
			}
			EVENTS_DEF_DEFAULT();
				break;
		}
	}

	void SimulationManager::constructSimulations()
	{
		auto motionSim = m_layer->add<Simulation::Motion>(MOTION_SIM, Simulation::Motion::Params());
		auto gravSim = m_layer->add<Simulation::Gravity>(GRAVITY_SIM, Simulation::Gravity::Params(), &motionSim->data);

		std::vector<std::vector<Simulation::Task>> emptyOrder = { {} };

		m_layer->setSimOrder(MOTION_SIM, {});
		m_layer->setSimOrder(GRAVITY_SIM, {});
	}

	std::vector<Simulation::Motion::Data>& SimulationManager::getMotionData()
	{
		return m_layer->get<Simulation::Motion>(MOTION_SIM)->data.getData();
	}
}