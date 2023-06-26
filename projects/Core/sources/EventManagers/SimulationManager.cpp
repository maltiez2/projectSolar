#include "pch.h"

#include "SimulationManager.h"
#include "CommunicationManager.h"
#include "ECS/EntityComponentSystem.h"

#include <random>

namespace projectSolar::EventManagers
{
	
	SimulationManager::SimulationManager(std::shared_ptr<Simulation::SimulationRunner> simulation, const size_t& threadsNumber) :
		EventHandler(threadsNumber),
		m_simulation(simulation)
	{
		m_simulationEntity = Com::get().ECS->create();
		Com::get().ECS->insert<Components::SimSettings>(m_simulationEntity, 1.0, 1.0);
		Com::get().ECS->insert<Components::SimPerformance>(m_simulationEntity, 1ui64, 0.0f);
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
			EVENT_DEF(SET_TPS);
			{
				m_simulation->setTicksPerSecond(eventData.ticksPerSecond);
			}
			EVENT_DEF(GENERATE_DEBUG_DATA);
			{
				genDebugLayout(eventData.primary, eventData.secondary, eventData.ternary);
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

				auto& motionData = Com::get().ECS->get<Components::MotionSimulation>(eventData.objects[0]);
				auto& simDoubleBufferedData = m_simulation->getData()->celestialObjects.celestialObjects[motionData.objectClass];
				auto& simData = simDoubleBufferedData.getData();
				auto& simBuffer = simDoubleBufferedData.getBuffer();
				
				std::unique_lock wirteLock(m_simulation->getData()->celestialObjects.dataMutex);
				if (simDoubleBufferedData.getDataVersion(motionData.index) == motionData.version) // @TODO Possible bug when objects in buffer and data are different due to deletion and insertion
				{
					simData[motionData.index].value.position = { eventData.newX, eventData.newY, eventData.newZ };
					simData[motionData.index].value.velocity = { 0.0, 0.0, 0.0 };
					simBuffer[motionData.index].value.position = { eventData.newX, eventData.newY, eventData.newZ };
					simBuffer[motionData.index].value.velocity = { 0.0, 0.0, 0.0 };
				}
				break;
			}
			EVENT_DEF(SET_SIM_RUN_PARAMS);
			{
				m_simulation->setGranularity(eventData.granularity);
			}
			EVENT_DEF(START_SIMULATION);
			{
				m_simulation->start();
			}
			EVENT_DEF(STOP_SIMULATION);
			{
				m_simulation->stop();
			}
			EVENT_DEF(ADVANCE_SIMULATION);
			{
				m_simulation->advance(eventData.ticks);
			}
			EVENTS_DEF_DEFAULT();
				break;
		}
	}

	MotionData& SimulationManager::getMotionData()
	{
		return m_simulation->getData()->celestialObjects.celestialObjects;
	}
	const MotionData& SimulationManager::getMotionData() const
	{
		return m_simulation->getData()->celestialObjects.celestialObjects;
	}
	SimulationState SimulationManager::getSimulationState() const
	{
		return m_simulation->getState();
	}

	void SimulationManager::genDebugLayout(size_t primary, size_t secondary, size_t ternary)
	{
		//std::unique_lock lock(m_layer->m_dataMutex);

		//auto primaryMotionSim   = m_layer->get<Simulation::Motion>(PRIMARY_ATTRACTORS_MOTION);
		//auto secondaryMotionSim = m_layer->get<Simulation::Motion>(SECONDARY_ATTRACTORS_MOTION);
		//auto ternaryMotionSim  = m_layer->get<Simulation::Motion>(NOT_ATTRACTORS_MOTION);

		//primaryMotionSim->data.clear();
		//secondaryMotionSim->data.clear();
		//ternaryMotionSim->data.clear();

		//auto ECS = Com::get().ECS;
		//auto view = ECS->getView<Components::Dynamic>();
		//for (auto& entity : view)
		//{
		//	ECS->destroy(entity);
		//}

		//const double starMass = 1e7;
		//const double planetMass = 1e3;
		//const double moonMass = 1e-9;
		//const double smallMass = 1e-19;

		//const double planetOrbit = 1.0;
		//const double moonOrbit = 2e-4;
		//
		//const double speedMult = 3e2;
		//const double planetSpeed = 1e1 * speedMult;
		//const double moonSpeed = 8e0 * speedMult;

		//double planetAngle = 2 * std::numbers::pi / (double)primary;
		//double moonAngle = 2 * std::numbers::pi / (double)secondary;

		//Eigen::Vector3d rotationAxis{ 0, 0, 1 };
		//Eigen::Vector3d nullVector(0.0, 0.0, 0.0);

		//primaryMotionSim->data.reserve(primary + 1);
		//secondaryMotionSim->data.reserve(primary * secondary);
		//ternaryMotionSim->data.reserve(ternary + 1);

		//uint32_t mapObjId = 0;

		//// STAR
		//auto starDataIndex = primaryMotionSim->data.addElement({ starMass, nullVector, nullVector, nullVector });
		//auto starEntity = ECS->create();
		//ECS->insert<Components::CelestialObject>(starEntity, Components::LongTitle{ "Star" });
		//ECS->insert<Components::Dynamic>(starEntity, PRIMARY_ATTRACTORS_MOTION, starDataIndex);
		//ECS->insert<Components::MapObject>(starEntity, mapObjId, 1.0f, 1.0f, 0.0f, 1.0f);
		//ECS->insert<Components::Player>(starEntity);
		//mapObjId++;

		//// PLANETS
		//for (size_t planetIndex = 0; planetIndex < primary; planetIndex++)
		//{
		//	Eigen::AngleAxisd planetRotation(planetAngle * (double)planetIndex, rotationAxis);
		//	Eigen::Vector3d planetOrbitRadius{ planetOrbit * (double)(planetIndex + 1), 0, 0 };
		//	Eigen::Vector3d planetOrbitSpeed{ 0, planetSpeed / std::sqrt((double)planetIndex + 1.0), 0 };
		//	
		//	planetOrbitRadius = planetRotation * planetOrbitRadius;
		//	planetOrbitSpeed = planetRotation * planetOrbitSpeed;

		//	auto planetDataIndex = primaryMotionSim->data.addElement({ planetMass, planetOrbitRadius, planetOrbitSpeed, nullVector });
		//	auto planetEntity = ECS->create();
		//	ECS->insert<Components::Dynamic>(planetEntity, PRIMARY_ATTRACTORS_MOTION, planetDataIndex);
		//	ECS->insert<Components::MapObject>(planetEntity, mapObjId, 1.0f, 1.0f, 1.0f, 1.0f);
		//	mapObjId++;

		//	// MOONS
		//	for (size_t moonIndex = 0; moonIndex < secondary; moonIndex++)
		//	{
		//		Eigen::AngleAxisd moonRotation(-moonAngle * (double)moonIndex, rotationAxis);
		//		Eigen::Vector3d moonOrbitRadius{ moonOrbit * (double)(moonIndex + 1), 0, 0 };
		//		Eigen::Vector3d moonOrbitSpeed{ 0, moonSpeed / std::sqrt((double)moonIndex + 1.0), 0 };

		//		moonOrbitRadius = planetOrbitRadius + moonRotation * moonOrbitRadius;
		//		moonOrbitSpeed = planetOrbitSpeed + moonRotation * moonOrbitSpeed;

		//		auto moonDataIndex = secondaryMotionSim->data.addElement({ moonMass, moonOrbitRadius, moonOrbitSpeed, nullVector });
		//		auto moonEntity = ECS->create();
		//		ECS->insert<Components::Dynamic>(moonEntity, SECONDARY_ATTRACTORS_MOTION, moonDataIndex);
		//		ECS->insert<Components::MapObject>(moonEntity, mapObjId, 0.5f, 0.5f, 0.5f, 1.0f);
		//		mapObjId++;
		//	}
		//}

		//// ASTEROIDS
		//std::random_device randomDevice;
		//std::mt19937 generator(randomDevice());
		//std::uniform_real_distribution orbitDistribution(1.0, (double)(primary + 2));
		//std::uniform_real_distribution angleDistribution(0.0, 2.0 * std::numbers::pi);
		//
		//for (size_t asteroidIndex = 0; asteroidIndex < ternary; asteroidIndex++)
		//{
		//	double orbit = orbitDistribution(generator);
		//	Eigen::AngleAxisd rotation(angleDistribution(generator), rotationAxis);
		//	Eigen::Vector3d orbitRadius{ planetOrbit * orbit, 0, 0};
		//	Eigen::Vector3d orbitSpeed{ 0, planetSpeed / std::sqrt(orbit), 0 };

		//	orbitRadius = rotation * orbitRadius;
		//	orbitSpeed = rotation * orbitSpeed;

		//	auto dataIndex = ternaryMotionSim->data.addElement({ smallMass, orbitRadius, orbitSpeed, nullVector });
		//	auto entity = ECS->create();
		//	ECS->insert<Components::Dynamic>(entity, NOT_ATTRACTORS_MOTION, dataIndex);
		//	ECS->insert<Components::MapObject>(entity, mapObjId, 0.3f, 0.3f, 0.7f, 0.5f);
		//	mapObjId++;
		//}

		//EMIT_EVENT(SIMULATION_UPDATED);
	}
}