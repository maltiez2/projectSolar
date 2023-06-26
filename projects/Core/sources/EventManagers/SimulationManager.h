#pragma once

#include "EventHandler.h"
#include "Simulation.h"
#include "Logger.h"

#include <memory>
#include <array>
#include <vector>
#include <EnTT/entt.hpp>


namespace projectSolar::EventManagers
{
	using MotionData = std::array<DataStructures::DoubleBuffVectorWithVersions<Simulation::ClestialObjectsData::MotionData>, Simulation::ClestialObjectsData::CELESTIAL_OBJECTS_CLASSES_NUMBER>;
	using MotionDataId = Simulation::ClestialObjectsData::ObjectId;
	using SimulationState = Simulation::SimulationRunner::State;

	class SimulationManager : public Events::EventHandler
	{
	public:
		SimulationManager(std::shared_ptr<Simulation::SimulationRunner> simulation, const size_t& threadsNumber = 1);
		~SimulationManager() override;

		MotionData& getMotionData();
		const MotionData& getMotionData() const;
		SimulationState getSimulationState() const;

		static const size_t maxObjDragged = 8;

		EVENT_DECL(SIMULATION_UPDATED, 1, float secondsPerStep; size_t stepsPerFrame);

		EVENT_DECL(SET_TPS, 101, uint32_t ticksPerSecond);
		EVENT_DECL(GENERATE_DEBUG_DATA, 102, size_t primary; size_t secondary; size_t ternary);
		EVENT_DECL(OBJ_DRUGGED, 103, float newX; float newY; float newZ; std::array<entt::entity, maxObjDragged> objects);
		EVENT_DECL(SET_SIM_RUN_PARAMS, 104, uint32_t granularity);

		EVENT_DECL(START_SIMULATION, 111);
		EVENT_DECL(STOP_SIMULATION, 112);
		EVENT_DECL(ADVANCE_SIMULATION, 113, uint32_t ticks);

	private:
		std::shared_ptr<Simulation::SimulationRunner> m_simulation;
		entt::entity m_simulationEntity;

		void processEvent(uint8_t eventType, uint8_t* data) override;

		void genDebugLayout(size_t primary, size_t secondary, size_t ternary);
	};
}