#pragma once

#include "Layer.h"
#include "Simulation.h"

#include <chrono>
#include <deque>
#include <shared_mutex>

namespace projectSolar::EventManagers
{
	class SimulationManager;
}

namespace projectSolar::Layers
{
	class StepsDivider
	{
	public:
		struct Params
		{
			size_t desiredStepsNumber;
			float timeRestrictionSeconds;
		};
		
		struct StepData
		{
			size_t stepsNumber;
			float timeRestrictionSeconds;
			float secondsPerStep;
			float excessTime;
		};

		size_t onRunStart(const Params& params);
		float onRunEnd();

	private:
		const uint8_t m_maxGrowFactor = 10;
		const uint8_t m_queieSize = 32;
		
		Params m_params;
		std::deque<StepData> m_results = {};
		std::chrono::time_point<std::chrono::steady_clock> m_startTimepoint;
		size_t m_currentStepNumber;
		
	};
	
	class SimLayer : public Layer, public EntityStack<Simulation::Simulation>
	{
	public:
		struct Params
		{
			double stepSize = 1.0;
			float timeRestriction = 0.5f / 144.0f;
		};
		
		explicit SimLayer(const Params& params);
		~SimLayer() override = default;

		void save(const std::string& filePath) override;
		void load(const std::string& filePath) override;

		void process() override;
		void onEvent(projectSolar::Graphics::InputEvent* ev) override;
		void generateDebugLayout(size_t motionId, size_t gravityId);

		size_t getLastStepsNumber() const;
		float getLastStepTime() const;
		std::vector<Simulation::Task>& getOrder(size_t id);

		void setStepSize(double stepSize);
		void setTimeRest(double timeRestrictionSeconds);
		void setSimOrder(size_t id, const std::vector<Simulation::Task>& order);
		void addToSimOrder(size_t id, const Simulation::Task& order);
		void excludeFromSimOrder(size_t id, const Simulation::Task& order);

	private:
		Params m_params;
		size_t m_lastStepsNumber;
		float  m_lastStepTime;
		std::shared_mutex m_dataMutex;

		std::map<size_t, std::vector<Simulation::Task>> m_simOrders;

		Simulation::SimulationRunner m_runner;
		StepsDivider m_stepsDivider;

		friend class EventManagers::SimulationManager;
	};
}