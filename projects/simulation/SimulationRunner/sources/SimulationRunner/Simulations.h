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
			double gravitationalConstant;
			double stepSize;
			uint8_t splittingFactor = 10;
			uint8_t unusedCores = 1;
		};

		explicit NBodySimulation(DataManager& dataManager);

		void run(SimulationParams params);
	};
}