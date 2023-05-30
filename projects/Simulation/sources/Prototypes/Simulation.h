#pragma once

#include "DoubleBufferedContainer.h"

#include <Eigen/Eigen>

#include <array>


namespace projectSolar::Simulation
{
	struct Task
	{
		size_t type;
		size_t first;
		size_t last;

		size_t size() const;
	};

	struct RunParams
	{
		size_t granularity = 1;
		size_t minimumSize = 1;
	};
	
	struct MotionData
	{
		double mass;
		Eigen::Vector3d position;
		Eigen::Vector3d velocity;
		Eigen::Vector3d force;
	};

	struct GravityData
	{
		Eigen::Vector3d force;
	};

	class Simulation
	{
	public:
		double stepSize = 1e0;
		double gravConst = 1e0;

		enum CalculationType : size_t
		{
			GRAVITY_STARS_AND_PLANETS,
			GRAVITY_MOONS_AND_ASTEROIDS,
			GRAVITY_ASTEROIDS_AND_ARTIFICIAL,
			MOTION_STARS_AND_PLANETS,
			MOTION_MOONS_AND_ASTEROIDS,
			MOTION_ASTEROIDS_AND_ARTIFICIAL
		};
		
		enum CelestialObjectsClass : size_t
		{
			STARS_AND_PLANETS,
			MOONS_AND_ASTEROIDS,
			ASTEROIDS_AND_ARTIFICIAL,
			CLASSES_NUMBER
		};
		
		std::array<DataStructures::DoubleBuffVector<MotionData>, CLASSES_NUMBER> celestialObjectsByClass_Motion;
		std::array<DataStructures::DoubleBuffVector<GravityData>, CLASSES_NUMBER> celestialObjectsByClass_Gravity;

		void calc(Task task);
		std::vector<Task> task(size_t type);
		bool skip(size_t type, size_t step);
		void swap(size_t step);
		void save(const std::string& saveName);
		void load(const std::string& saveName);
		RunParams runParams(size_t type) const;

	private:
		void sumForces(Task task, size_t objectClass);
		
		void calc_motion(Task task, size_t objectClass);
		void calc_gravity(Task task, size_t objectClass);

		std::vector<Task> task_motion(size_t objectClass, size_t type);
		std::vector<Task> task_gravity(size_t objectClass, size_t type);

		bool skip_motion(size_t objectClass, size_t step);
		bool skip_gravity(size_t objectClass, size_t step);
	};

	
}