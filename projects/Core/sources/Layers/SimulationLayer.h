#pragma once

#include "Layer.h"
#include "Simulation.h"

#include <chrono>
#include <queue>

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
		const uint8_t m_queieSize = 10;
		
		Params m_params;
		std::queue<StepData> m_results = {}; // @TODO Experiment with algorithms that us this
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
		~SimLayer() override;

		void process() override;
		void onEvent(Graphics::InputEvent* ev) override;

		size_t getLastStepsNumber() const;
		float getLastStepTime() const;

		void setStepSize(double stepSize);
		void setTimeRest(double timeRestrictionSeconds);
		void setSimOrder(size_t id, const std::vector<Simulation::Task>& order);
		void addToSimOrder(size_t id, const Simulation::Task& order);
		void excludeFromSimOrder(size_t id, const Simulation::Task& order);

	private:
		Params m_params;
		size_t m_lastStepsNumber;
		float  m_lastStepTime;

		std::map<size_t, std::vector<Simulation::Task>> m_simOrders;

		Simulation::SimulationRunner m_runner;
		StepsDivider m_stepsDivider;
	};
}