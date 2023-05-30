#include "pch.h"

#include "Simulation.h"

#include "Logger.h"


namespace projectSolar::Simulation
{
	void Simulation::calc_gravity(Task task, size_t objectClass)
	{
		auto& initial = celestialObjectsByClass_Motion[objectClass].getData();
		auto& result = celestialObjectsByClass_Gravity[objectClass].getBuffer();

		for (size_t index = task.first; index <= task.last; index++)
		{
			result[index].force = { 0.0, 0.0, 0.0 };

			for (size_t objClass = 0; objClass <= objectClass; objClass++)
			{
				auto& attractorsData = celestialObjectsByClass_Motion[objClass];

				for (size_t objIndex = 0; objIndex < attractorsData.size(); objIndex++)
				{
					if (!attractorsData.isValid(objIndex))
					{
						continue;
					}
					if (objIndex == index)
					{
						continue;
					}

					const auto& attractor = attractorsData.getData()[objIndex];

					Eigen::Vector3d direction = attractor.position - initial[index].position;
					double distance = direction.norm();
					result[index].force += direction * attractor.mass * initial[index].mass / distance / distance / distance;
				}
			}

			result[index].force = result[index].force * gravConst;
		}
	}
	std::vector<Task> Simulation::task_gravity(size_t objectClass, size_t type)
	{
		return { { type, 0, celestialObjectsByClass_Gravity[objectClass].size() - 1 } };
	}
	bool Simulation::skip_gravity(size_t objectClass, size_t step)
	{
		return false;
	}
}
