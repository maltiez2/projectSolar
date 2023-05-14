#pragma once

#include "DataManager.h"


namespace projectSolar::Simulation
{
	class BaseSimulation
	{
	public:
		explicit BaseSimulation(DataManager& dataManager);

	protected:
		DataManager& m_simulationData;
	};

	class NBodySimulation : public BaseSimulation
	{
	public:
		struct SimulationParams
		{
			double gravitationalConstant = 1.0;
			double stepSize = 1.0;
			uint8_t splittingFactor = 10;
			uint8_t unusedCores = 1;
		};

		explicit NBodySimulation(DataManager& dataManager);

		void run(SimulationParams params);
	};

	class BasicCollisionSimulation : public BaseSimulation
	{
	public:
		struct SimulationParams
		{
			uint8_t splittingFactor = 10;
			uint8_t unusedCores = 1;
		};

		explicit BasicCollisionSimulation(DataManager& dataManager);

		void run(SimulationParams params);
	};
}