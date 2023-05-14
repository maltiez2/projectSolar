#include "SimulationLayer.h"
#include "GameLogic/CommunicationManager.h"

#include <algorithm>

namespace projectSolar::Layers
{
	// Sim layer
	SimLayer::SimLayer(const Params& params) :
		m_params(params)
	{

	}
	SimLayer::~SimLayer()
	{
	}

	void SimLayer::process()
	{
		m_lastStepsNumber = m_stepsDivider.onRunStart({ 10, m_params.timeRestriction });
		double stepSize = m_params.stepSize / (double)m_lastStepsNumber;

		for (size_t step = 0; step < m_lastStepsNumber; step++)
		{
			for (const auto& [id, simulation] : m_attached)
			{
				LOG_ASSERT(m_simOrders.contains(id), "Add orders for simulation: ", id);
				simulation->stepSize = stepSize;
				m_runner.run({ 5, 1 }, simulation, m_simOrders[id]);
			}

			for (const auto& [id, simulation] : m_attached)
			{
				simulation->swapData();
			}
		}

		m_lastStepTime = m_stepsDivider.onRunEnd();

		EMIT_EVENT(SIMULATION_UPDATED, m_lastStepTime, m_lastStepsNumber);

		LOG_INFO("[SimLayer] Steps per frame: ", m_lastStepsNumber, ", milliseconds per step: ", m_lastStepTime * 1000.0, ", milliseconds per frame: ", (float)m_lastStepsNumber * m_lastStepTime * 1000.0);
	}
	void SimLayer::onEvent(Graphics::InputEvent* ev)
	{
		// No events to process
	}
	size_t SimLayer::getLastStepsNumber() const
	{
		return m_lastStepsNumber;
	}
	float SimLayer::getLastStepTime() const
	{
		return m_lastStepTime;
	}
	void SimLayer::setStepSize(double stepSize)
	{
		m_params.stepSize = stepSize;
	}
	void SimLayer::setTimeRest(double timeRestrictionSeconds)
	{
		m_params.timeRestriction = timeRestrictionSeconds;
	}
	void SimLayer::setSimOrder(size_t id, const std::vector<Simulation::Task>& order)
	{
		m_simOrders[id] = order;
	}


	// Steps divider
	size_t StepsDivider::onRunStart(const Params& params)
	{
		m_startTimepoint = std::chrono::steady_clock::now();
		m_params = params;

		if (m_results.empty())
		{
			m_currentStepNumber = m_params.desiredStepsNumber;
			return m_currentStepNumber;
		}

		const auto& prev = m_results.back();
		float excessTime = prev.excessTime + prev.timeRestrictionSeconds - m_params.timeRestrictionSeconds;
		float excessSteps = excessTime / prev.secondsPerStep;
		excessSteps = excessSteps + 0.1f * std::abs(excessSteps);
		size_t newStepsNumber = (10 * prev.stepsNumber - (size_t)(excessSteps * 10.0f))/10;

		m_currentStepNumber = std::min(std::max(newStepsNumber, 1ui64), m_params.desiredStepsNumber);
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
			m_results.pop();
		}

		m_results.emplace(
			m_currentStepNumber,
			m_params.timeRestrictionSeconds,
			secondsPerStep,
			excessTimeSeconds
		);

		return secondsPerStep;
	}
}