#pragma once

#include <chrono>
#include <deque>


namespace projectSolar::Simulation
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
}