#pragma once

#include <stdint.h>

namespace projectSolar::Simulation
{
	class SimulationData;
	struct Task;
	
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
