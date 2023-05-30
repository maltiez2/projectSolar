#include "pch.h"

#include "Simulation.h"

#include "Logger.h"


namespace projectSolar::Simulation
{
	void Simulation::sumForces(Task task, size_t objectClass)
	{
		for (size_t index = task.first; index <= task.last; index++)
		{
			auto& object = celestialObjectsByClass_Motion[objectClass].getBuffer()[index];
			object.force = celestialObjectsByClass_Gravity[objectClass].getData()[index].force;
		}
	}
	void Simulation::calc_motion(Task task, size_t objectClass)
	{
		auto& initial = celestialObjectsByClass_Motion[objectClass].getData();
		auto& result = celestialObjectsByClass_Motion[objectClass].getBuffer();

		for (size_t index = task.first; index <= task.last; index++)
		{
			const auto& initialData = initial[index];
			auto& resultData = result[index];

			resultData.position = initialData.position + stepSize * initialData.velocity;
			resultData.velocity = initialData.velocity + stepSize * initialData.force / initialData.mass;
		}
	}
	std::vector<Task> Simulation::task_motion(size_t objectClass, size_t type)
	{
		return { { type, 0, celestialObjectsByClass_Motion[objectClass].size() - 1 } };
	}
	bool Simulation::skip_motion(size_t objectClass, size_t step)
	{
		return false;
	}
}
