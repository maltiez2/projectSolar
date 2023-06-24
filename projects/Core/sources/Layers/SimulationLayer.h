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
	class SimLayer : public Layer, public Simulation::SimulationStack
	{
	public:		
		const double stepSize = 1e-8;
		
		struct Params
		{
			double simulationTimePerSecond = 1.0;
			float timeRestriction = 0.5f / 144.0f;
		};
		
		explicit SimLayer(const Params& params);
		~SimLayer() override = default;

		void save(const std::string& filePath) override;
		void load(const std::string& filePath) override;

		void process() override;
		void onEvent(projectSolar::Graphics::InputEvent* ev) override;

		size_t getLastStepsNumber() const;
		float getLastStepTime() const;

		void setTimeRest(double timeRestrictionSeconds);
		void setTimePerSecond(double simulationTimePerSecond);
		void setSecondsPerFrame(double secondsPerFrame);

	private:
		Params m_params;
		size_t m_lastStepsNumber;
		float  m_lastStepTime;
		double  m_secondsPerFrame;
		std::shared_mutex m_dataMutex;

		Simulation::StepsDivider m_stepsDivider;

		friend class EventManagers::SimulationManager;
	};
}