#pragma once

#include <memory>

namespace projectSolar::Simulation
{
	class Simulation;
	class ThreadPool;
	class SimulationStepper;
	class SimulationData;

	/*
		Stores simulation and thread pool.
		Provides API for Core to interact with simulation (get and set data, set parameters).
		Runs simulation with given target TPS.
	*/
	class SimulationRunner
	{
	public:
		SimulationRunner();
		~SimulationRunner();

		void run();

		SimulationData& getData();

	private:
		std::shared_ptr<Simulation> m_simulation;
		std::shared_ptr<ThreadPool> m_threadPool;
		std::shared_ptr<SimulationStepper> m_stepper;
	};
}