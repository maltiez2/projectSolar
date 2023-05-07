#pragma once

#include "SimulationRunner/DoubleBufferedContainer.h"
#include "SimulationRunner/DataManager.h"
#include "SimulationRunner/Simulations.h"
#include "SimulationRunner/Solvers.h"

#include <queue>
#include <chrono>




namespace projectSolar::Simulation
{	
	class SimulationRunner
	{
	public:
		struct Params
		{
			double gravitationalConstant;
			double stepSize;
			float framePeriodFactor;
			uint8_t framesPerSecond;
			uint16_t defaultStepsNumber;
			float stepsDiffBias = -0.1f;
		};

		SimulationRunner();

		float run(const Params& params);
		DataManager& getData();

	private:
		class frameRateConsistensyController
		{
		public:
			struct Params
			{
				uint16_t stepsNumber;
				uint8_t framesPerSecond;
				float stepsPerSecond;
				float excessTime;
			};

			uint16_t onRunStart(const SimulationRunner::Params& runnerParams);
			float onRunEnd();

		private:
			const uint8_t m_maxGrowFactor = 10;
			const uint8_t m_queieSize = 10;
			std::queue<Params> m_results = {};
			std::chrono::time_point<std::chrono::steady_clock> m_startTimepoint;
			uint16_t m_currentStepNumber;
			SimulationRunner::Params m_currentRunnerParams;
		};
		
		DataManager m_simulationData;
		NBodySimulation m_simulation_nBody;
		frameRateConsistensyController m_frameRateController;
	};
}
