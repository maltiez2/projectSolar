#pragma once

namespace projectSolar::Simulation
{
	class SimulationData;

	/*
		Used to tell worker thread what system to process and on what data.
		Contains system id and parallelization parameters.
		Parameters have different meaning for different systems.
	*/
	struct Task
	{
		uint16_t system;
		uint64_t param_1;
		uint64_t param_2;
		uint64_t param_3;
	};
	
	/*
		Solves motion equations for celestial objects.
	*/
	class MotionSystem
	{
	public:
		struct Params
		{
			double stepSize;
			double gravConst;
		};

		MotionSystem(Params params, SimulationData& simulationData);
		~MotionSystem() = default;

		/*
			Uses Midpoint method (Second-order Runge-Kutta method with 2 stages) to solve motion equations
			@param const Task& task - defines object class and first and last object indexes to solve for
		*/
		void solve(const Task& task) const;

	private:
		Params m_params;
		SimulationData& m_data;
	};
}
