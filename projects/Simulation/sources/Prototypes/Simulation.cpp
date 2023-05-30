#include "pch.h"

#include "Simulation.h"
#include "Serializer.h"

#include "Logger.h"


namespace projectSolar::Simulation
{
	size_t Task::size() const
	{
		if (first > last)
		{
			return 0;
		}

		return last - first + 1;
	}
	
	void Simulation::calc(Task task)
	{
		switch (task.type)
		{
			// **************** GRAVITY *******************
		case GRAVITY_STARS_AND_PLANETS:
			calc_gravity(task, STARS_AND_PLANETS);
			break;
			
		case GRAVITY_MOONS_AND_ASTEROIDS:
			calc_gravity(task, MOONS_AND_ASTEROIDS);
			break;
			
		case GRAVITY_ASTEROIDS_AND_ARTIFICIAL:
			calc_gravity(task, ASTEROIDS_AND_ARTIFICIAL);
			break;
			
			// **************** MOTION *******************
		case MOTION_STARS_AND_PLANETS:
			sumForces(task, STARS_AND_PLANETS);
			calc_motion(task, STARS_AND_PLANETS);
			break;
			
		case MOTION_MOONS_AND_ASTEROIDS:
			sumForces(task, MOONS_AND_ASTEROIDS);
			calc_motion(task, MOONS_AND_ASTEROIDS);
			break;
			
		case MOTION_ASTEROIDS_AND_ARTIFICIAL:
			sumForces(task, ASTEROIDS_AND_ARTIFICIAL);
			calc_motion(task, ASTEROIDS_AND_ARTIFICIAL);
			break;
			
		default:
			LOG_ASSERT(false, "[Simulation] Unknown simulation type");
			break;
		}
	}
	std::vector<Task> Simulation::task(size_t type)
	{
		switch (type)
		{
			// **************** GRAVITY *******************
		case GRAVITY_STARS_AND_PLANETS:
			return task_gravity(STARS_AND_PLANETS, type);

		case GRAVITY_MOONS_AND_ASTEROIDS:
			return task_gravity(MOONS_AND_ASTEROIDS, type);

		case GRAVITY_ASTEROIDS_AND_ARTIFICIAL:
			return task_gravity(ASTEROIDS_AND_ARTIFICIAL, type);

			// **************** MOTION *******************
		case MOTION_STARS_AND_PLANETS:
			return task_motion(STARS_AND_PLANETS, type);

		case MOTION_MOONS_AND_ASTEROIDS:
			return task_motion(MOONS_AND_ASTEROIDS, type);

		case MOTION_ASTEROIDS_AND_ARTIFICIAL:
			return task_motion(ASTEROIDS_AND_ARTIFICIAL, type);

		default:
			LOG_ASSERT(false, "[Simulation] Unknown simulation type");
			break;
		}
	}
	bool Simulation::skip(size_t type, size_t step)
	{
		switch (type)
		{
			// **************** GRAVITY *******************
		case GRAVITY_STARS_AND_PLANETS:
			return skip_gravity(STARS_AND_PLANETS, step);

		case GRAVITY_MOONS_AND_ASTEROIDS:
			return skip_gravity(MOONS_AND_ASTEROIDS, step);

		case GRAVITY_ASTEROIDS_AND_ARTIFICIAL:
			return skip_gravity(ASTEROIDS_AND_ARTIFICIAL, step);

			// **************** MOTION *******************
		case MOTION_STARS_AND_PLANETS:
			return skip_motion(STARS_AND_PLANETS, step);

		case MOTION_MOONS_AND_ASTEROIDS:
			return skip_motion(MOONS_AND_ASTEROIDS, step);

		case MOTION_ASTEROIDS_AND_ARTIFICIAL:
			return skip_motion(ASTEROIDS_AND_ARTIFICIAL, step);

		default:
			LOG_ASSERT(false, "[Simulation] Unknown simulation type");
			break;
		}
	}
	void Simulation::swap(size_t step)
	{
		for (size_t objClass = 0; objClass < CLASSES_NUMBER; objClass++)
		{
			celestialObjectsByClass_Motion[objClass].swap();
			celestialObjectsByClass_Gravity[objClass].swap();
		}
	}
	void Simulation::save(const std::string& saveName)
	{
		Serializer serializer;
		errno_t error = serializer.open<Serializer::fileMode::writeBytes>(saveName + ".sim");
		if (error != 0)
		{
			LOG_ERROR("[SimLayer] Cannot open file for writing '", saveName, ".sim', error: ", error);
			return;
		}
		for (size_t objClass = 0; objClass < CLASSES_NUMBER; objClass++)
		{
			serializer.serialize(celestialObjectsByClass_Motion[objClass]);
			serializer.serialize(celestialObjectsByClass_Gravity[objClass]);
		}
		serializer.close();
	}
	void Simulation::load(const std::string& saveName)
	{
		Serializer serializer;
		errno_t error = serializer.open<Serializer::fileMode::readBytes>(saveName + ".sim");
		if (error != 0)
		{
			LOG_ERROR("[SimLayer] Cannot open file for reading '", saveName, ".sim', error: ", error);
			return;
		}
		for (size_t objClass = 0; objClass < CLASSES_NUMBER; objClass++)
		{
			serializer.deserialize(celestialObjectsByClass_Motion[objClass]);
			serializer.deserialize(celestialObjectsByClass_Gravity[objClass]);
		}
		serializer.close();
	}
	RunParams Simulation::runParams(size_t type) const
	{
		switch (type)
		{
			// **************** GRAVITY *******************
		case GRAVITY_STARS_AND_PLANETS:
			return { 1, 1 };

		case GRAVITY_MOONS_AND_ASTEROIDS:
			return { 3, 1 };

		case GRAVITY_ASTEROIDS_AND_ARTIFICIAL:
			return { 9, 1 };

			// **************** MOTION *******************
		case MOTION_STARS_AND_PLANETS:
			return { 1, 1 };

		case MOTION_MOONS_AND_ASTEROIDS:
			return { 1, 1 };

		case MOTION_ASTEROIDS_AND_ARTIFICIAL:
			return { 1, 1 };

		default:
			LOG_ASSERT(false, "[Simulation] Unknown simulation type");
			break;
		}
	}
}
