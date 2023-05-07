#include "pch.h"

#include "SimulationRunner.h"


namespace projectSolar::Simulation
{
	// DATA MANAGER
	void DataManager::swap()
	{
		attractorsData.swap();
		attractantsData.swap();
		propulsedData.swap();
	}
	errno_t DataManager::save(std::string_view filePath)
	{
		Serializer serializer;
		errno_t error = serializer.open<Serializer::fileMode::writeBytes>(filePath);
		if (error)
		{
			LOG_ERROR(std::string("[DataManager] Error on opening file for writing: ") + filePath.data());
			return error;
		}
		serializer.serialize(attractorsData);
		serializer.serialize(attractantsData);
		serializer.serialize(propulsedData);
		serializer.close();
		return 0;
	}
	errno_t DataManager::load(std::string_view filePath)
	{
		Serializer serializer;
		errno_t error = serializer.open<Serializer::fileMode::readBytes>(filePath);
		if (error)
		{
			LOG_ERROR(std::string("[DataManager] Error on opening file for reading: ") + filePath.data());
			return error;
		}
		serializer.deserialize(attractorsData);
		serializer.deserialize(attractantsData);
		serializer.deserialize(propulsedData);
		serializer.close();
		return 0;
	}

	// SIMULATION
	BaseSimulation::BaseSimulation(DataManager& dataManager) :
		m_simulationData(dataManager)
	{
	}

	NBodySimulation::NBodySimulation(DataManager& dataManager) :
		BaseSimulation(dataManager)
	{
	}
	void NBodySimulation::run(SimulationParams params)
	{
		PROFILE_FUNCTION();
		
		auto partitioner = oneapi::tbb::affinity_partitioner();

		uint8_t splittingFactor = params.splittingFactor;
		uint8_t concurrencyFactor = std::max(oneapi::tbb::info::default_concurrency() - params.unusedCores, 1);
		auto tbbGlobalControl = tbb::global_control(tbb::global_control::max_allowed_parallelism, concurrencyFactor);

		oneapi::tbb::task_arena arena(concurrencyFactor);
		const size_t attractorsAmount = m_simulationData.attractorsData.size();
		const size_t attractantsAmount = m_simulationData.attractantsData.size();
		const size_t propulsedAmount = m_simulationData.propulsedData.size();

		size_t attractantsGrainSize = std::max(attractantsAmount / concurrencyFactor / splittingFactor, (size_t)1);
		size_t attractorsGrainSize = std::max(attractorsAmount / concurrencyFactor / std::max(splittingFactor / 3, 3), (size_t)1);
		size_t propulsedGrainSize = std::max(propulsedAmount / concurrencyFactor / std::max(splittingFactor / 3, 3), (size_t)1);

		oneapi::tbb::parallel_for(
			oneapi::tbb::blocked_range<size_t>(0, attractantsAmount, attractantsGrainSize),
			SolverParallelWrapper<Components::Attractant::Data, Components::Attractor::Data, EulerSolver>(
				{ params.gravitationalConstant, params.stepSize },
				m_simulationData.attractantsData,
				m_simulationData.attractorsData,
				false
				),
			partitioner
		);

		oneapi::tbb::parallel_for(
			oneapi::tbb::blocked_range<size_t>(0, attractorsAmount, attractorsGrainSize),
			SolverParallelWrapper<Components::Attractor::Data, Components::Attractor::Data, EulerSolver>(
				{ params.gravitationalConstant, params.stepSize },
				m_simulationData.attractorsData,
				m_simulationData.attractorsData,
				true
				),
			partitioner
		);

		oneapi::tbb::parallel_for(
			oneapi::tbb::blocked_range<size_t>(0, propulsedAmount, propulsedGrainSize),
			SolverParallelWrapper<Components::Propulsed::Data, Components::Attractor::Data, EulerSolverPropulsed>(
				{ params.gravitationalConstant, params.stepSize },
				m_simulationData.propulsedData,
				m_simulationData.attractorsData,
				false
				),
			partitioner
		);
	}

	// RUNNER
	SimulationRunner::SimulationRunner() :
		m_simulation_nBody(m_simulationData)
	{
	}
	float SimulationRunner::run(const Params& params)
	{
		uint16_t stepsNumber = m_frameRateController.onRunStart(params);

		for (uint16_t index = 0; index < stepsNumber; index++)
		{
			m_simulation_nBody.run({ params.gravitationalConstant, params.stepSize / stepsNumber, 5, 1 });
			m_simulationData.swap();
		}

		float secondsElapsed = m_frameRateController.onRunEnd();

		return secondsElapsed;
	}
	DataManager& SimulationRunner::getData()
	{
		return m_simulationData;
	}

	uint16_t SimulationRunner::frameRateConsistensyController::onRunStart(const SimulationRunner::Params& runnerParams)
	{
		m_startTimepoint = std::chrono::steady_clock::now();
		m_currentRunnerParams = runnerParams;
		
		if (m_results.empty())
		{
			LOG_DEBUG("[SimulationRunner] [frameRateConsistensyController] onRunStart - defaultStepsNumber: " + std::to_string(m_currentRunnerParams.defaultStepsNumber));
			m_currentStepNumber = m_currentRunnerParams.defaultStepsNumber;
			return m_currentRunnerParams.defaultStepsNumber;
		}

		float frameRateDiffFactor = (float)m_currentRunnerParams.framesPerSecond / (float)m_results.back().framesPerSecond;
		float stepsDelta = m_results.back().excessTime * m_results.back().stepsPerSecond;
		float stepsNumber = stepsDelta + m_results.back().stepsNumber + m_currentRunnerParams.stepsDiffBias * std::abs(stepsDelta);
		float reducedStepsNumber = stepsNumber * frameRateDiffFactor;

		LOG_DEBUG("[SimulationRunner] [frameRateConsistensyController] onRunStart - stepsNumber: " + std::to_string(std::max((uint16_t)reducedStepsNumber, (uint16_t)1)) + ", stepsDelta: " + std::to_string(stepsDelta));

		m_currentStepNumber = std::min(std::max((uint16_t)reducedStepsNumber, (uint16_t)1), (uint16_t)(m_results.back().stepsNumber * m_maxGrowFactor));
		return m_currentStepNumber;
	}
	float SimulationRunner::frameRateConsistensyController::onRunEnd()
	{
		auto endTimepoint = std::chrono::steady_clock::now();

		uint64_t start = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_startTimepoint).time_since_epoch().count();
		uint64_t end = std::chrono::time_point_cast<std::chrono::nanoseconds>(endTimepoint).time_since_epoch().count();

		float frameTimeSeconds = (float)(end - start) * 1e-9f;
		float desiredFrameTime = m_currentRunnerParams.framePeriodFactor / m_currentRunnerParams.framesPerSecond;
		
		if (m_results.size() > m_queieSize)
		{
			m_results.pop();
		}

		LOG_DEBUG("[SimulationRunner] [frameRateConsistensyController] onRunEnd - frameTimeSeconds: " + std::to_string(frameTimeSeconds) + ", desiredFrameTime: " + std::to_string(desiredFrameTime));

		m_results.emplace(
			m_currentStepNumber,
			m_currentRunnerParams.framesPerSecond,
			m_currentStepNumber / frameTimeSeconds,
			desiredFrameTime - frameTimeSeconds
		);
		
		return frameTimeSeconds;
	}
}
