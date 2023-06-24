#include "pch.h"

#include "SimulationSystems.h"
#include "SimulationData.h"


namespace projectSolar::Simulation
{
	MotionSystem::MotionSystem(Params params, SimulationData& simulationData) :
		m_params(params),
		m_data(simulationData)
	{

	}

	void MotionSystem::solve(const Task& task) const
	{
		PROFILE_FUNCTION();
		
		const uint64_t& objectClass = task.param_1;
		const uint64_t& firstObject = task.param_2;
		const uint64_t& lastObject = task.param_3;

		LOG_ASSERT(objectClass < ClestialObjectsData::CELESTIAL_OBJECTS_CLASSES_NUMBER, " [MotionSystem] Wrong object class: ", objectClass);
		LOG_ASSERT(lastObject < m_data.celestialObjects.celestialObjects[objectClass].size(), " [MotionSystem] Wrong last object index: ", lastObject, ", for object class: ", objectClass);
		
		auto& celestialObjects = m_data.celestialObjects.celestialObjects;
		const auto& objectsData = celestialObjects[objectClass].getData();
		auto& objectsBuffer = celestialObjects[objectClass].getBuffer();

		for (size_t objectIndex = firstObject; objectIndex <= lastObject; objectIndex++)
		{
			const auto& objectInitial = objectsData[objectIndex].value;
			auto& objectResult = objectsBuffer[objectIndex].value;

			const Eigen::Vector3d& objectPosition = objectInitial.position;
			const Eigen::Vector3d  objectPositionHalfStep = objectPosition + 0.5 * m_params.stepSize * objectInitial.velocity;

			Eigen::Vector3d force = { 0.0, 0.0, 0.0 };
			Eigen::Vector3d forceHalfStep = { 0.0, 0.0, 0.0 };

			for (uint8_t attractorClass = 0; attractorClass <= objectClass; attractorClass++)
			{
				const auto& attractorsData = celestialObjects[attractorClass].getData();

				for (size_t attractorIndex = 0; attractorIndex < attractorsData.size(); attractorIndex++)
				{
					if (attractorClass == objectClass && attractorIndex == objectIndex)
					{
						continue;
					}

					Eigen::Vector3d direction;
					double distance;

					const auto& attractor = attractorsData[attractorIndex].value;

					direction = attractor.position - objectPosition;
					distance = direction.norm();
					force += direction * attractor.mass * objectInitial.mass / distance / distance / distance * m_params.gravConst;

					direction = attractor.position - objectPositionHalfStep;
					distance = direction.norm();
					forceHalfStep += direction * attractor.mass * objectInitial.mass / distance / distance / distance * m_params.gravConst;
				}
			}

			Eigen::Vector3d acceleration = (force + objectInitial.externalForce) / objectInitial.mass;
			Eigen::Vector3d accelerationHalfStep = (forceHalfStep + objectInitial.externalForce) / objectInitial.mass;

			objectResult.position = objectInitial.position + m_params.stepSize * objectInitial.velocity + 0.5 * m_params.stepSize * m_params.stepSize * acceleration;
			objectResult.velocity = objectInitial.velocity + m_params.stepSize * accelerationHalfStep;
		}
	}
}
