#include "pch.h"

#include "Simulation.h"

namespace projectSolar::Simulation
{
	// Simulation
	void Simulation::processTask(Task task)
	{
		switch (task.system)
		{
		case MOTION:
			motion.solve(task);
			break;
		default:
			LOG_ASSERT(false, " [Simulation] Unknown system id: ", task.system);
		}
	}
}
