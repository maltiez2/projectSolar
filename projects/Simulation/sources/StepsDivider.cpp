#include "pch.h"

#include "StepsDivider.h"

namespace projectSolar::Simulation
{
	size_t StepsDivider::onRunStart(const Params& params)
	{
		m_startTimepoint = std::chrono::steady_clock::now();
		m_params = params;

		if (m_results.empty())
		{
			m_currentStepNumber = 1;
			return m_currentStepNumber;
		}

		StepsDivider::StepData average(0, 0.0f, 0.0f, 0.0f);
		for (const auto& result : m_results)
		{
			average.excessTime += result.excessTime;
			average.secondsPerStep += result.secondsPerStep;
			average.stepsNumber += result.stepsNumber;
			average.timeRestrictionSeconds += result.timeRestrictionSeconds;
		}
		average.excessTime /= (float)m_results.size();
		average.secondsPerStep /= (float)m_results.size();
		average.stepsNumber /= m_results.size();
		average.timeRestrictionSeconds /= (float)m_results.size();

		float excessTime = average.excessTime + average.timeRestrictionSeconds - m_params.timeRestrictionSeconds;
		float excessSteps = excessTime / average.secondsPerStep;
		excessSteps = excessSteps + 0.1f * std::abs(excessSteps);
		int64_t newStepsNumber = (10 * (int64_t)average.stepsNumber - (int64_t)(excessSteps * 10.0f)) / 10;

		m_currentStepNumber = std::min((uint64_t)std::max(newStepsNumber, 1i64), m_params.desiredStepsNumber);
		return m_currentStepNumber;
	}
	float StepsDivider::onRunEnd()
	{
		auto endTimepoint = std::chrono::steady_clock::now();
		uint64_t start = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_startTimepoint).time_since_epoch().count();
		uint64_t end = std::chrono::time_point_cast<std::chrono::nanoseconds>(endTimepoint).time_since_epoch().count();

		float totalTimeSeconds = (float)(end - start) * 1e-9f;
		float excessTimeSeconds = totalTimeSeconds - m_params.timeRestrictionSeconds;
		float secondsPerStep = totalTimeSeconds / (float)m_currentStepNumber;

		if (m_results.size() > m_queieSize)
		{
			m_results.pop_front();
		}

		m_results.emplace_back(
			m_currentStepNumber,
			m_params.timeRestrictionSeconds,
			secondsPerStep,
			excessTimeSeconds
		);

		return secondsPerStep;
	}
}
