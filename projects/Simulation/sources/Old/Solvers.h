#pragma once

#include <Eigen/Core>
#include <oneapi/tbb.h>
#include <set>
#include <vector>

#include "DoubleBufferedContainer.h"

#include "Logger.h"


namespace projectSolar::Simulation
{
// Collision solver

	class BasicCollisionSolver
	{
	public:
		template<typename TargetData, typename ObjectData, typename ColliisionDataType>
		static void solve(
			std::set<size_t>& elementsToSkip,
			TargetData& targetData,
			TargetData& targetBuffer,
			ColliisionDataType& targetCollision, 
			DoubleBuffVector<ObjectData>& objectsData,
			DoubleBuffVector<ColliisionDataType>& objectsColliisionDataType
		)
		{
			double targetRadius = targetCollision.radius;
			Eigen::Vector3d targetPosition = targetData.position;
			
			for (size_t index = 0; index < objectsData.size(); index++)
			{
				if (elementsToSkip.contains(index))
				{
					continue;
				}

				Eigen::Vector3d radius = targetPosition - objectsData.getData()[index].position;

				if (radius.norm() < targetRadius + objectsColliisionDataType.getData()[index].radius)
				{
					double targetMass = targetCollision.mass;
					double objectMass = objectsColliisionDataType.getData()[index].mass;
					double  totalMass = targetMass + objectMass;
					Eigen::Vector3d radius = objectsData.getData()[index].position - targetData.position;
					radius.normalize();
					double targetVelocity = radius.transpose() * targetData.velocity;
					double objectVelocity = radius.transpose() * objectsData.getData()[index].velocity;
					if (objectVelocity > targetVelocity)
					{
						return;
					}
					double newVelocity = (targetMass - objectMass) / totalMass * targetVelocity + 2 * objectMass / totalMass * objectVelocity;
					double velocityDelta = newVelocity - targetVelocity;
					targetBuffer.velocity = targetData.velocity + radius * velocityDelta * 0.99f;
				}
			}
		}

		template<typename TargetData, typename ObjectData, typename ColliisionDataType>
		static bool detect(
			std::set<size_t>& elementsToSkip,
			TargetData& targetData,
			ColliisionDataType& targetCollision,
			DoubleBuffVector<ObjectData>& objectsData,
			DoubleBuffVector<ColliisionDataType>& objectsColliisionDataType,
			size_t& object
		)
		{
			double targetRadius = targetCollision.radius;
			Eigen::Vector3d targetPosition = targetData.position;

			for (size_t index = 0; index < objectsData.size(); index++)
			{
				if (elementsToSkip.contains(index))
				{
					continue;
				}

				Eigen::Vector3d radius = targetPosition - objectsData.getData()[index].position;
				
				if (radius.norm() < targetRadius + objectsColliisionDataType.getData()[index].radius)
				{
					object = index;
					return true;
				}
			}

			return false;
		}
	};

	template<typename SubjectDataType, typename ActorDataType, typename ColliisionDataType, typename Solver>
	class CollisionSolverParallelWrapper
	{
	public:
		CollisionSolverParallelWrapper(
			DoubleBuffVector<SubjectDataType>& attractants,
			DoubleBuffVector<ActorDataType>& attractors,
			DoubleBuffVector<ColliisionDataType>& attractantsCollideData,
			DoubleBuffVector<ColliisionDataType>& attractorsCollideData,
			bool sameData
		) :
			m_attractants(attractants),
			m_attractors(attractors),
			m_attractantsCollideData(attractantsCollideData),
			m_attractorsCollideData(attractorsCollideData),
			m_sameData(sameData)
		{
		}

		void operator()(const oneapi::tbb::blocked_range<size_t>& range) const
		{
			auto& attractantsData = m_attractants.getData();
			auto& attractantsBuffer = m_attractants.getBuffer();

			for (auto index = range.begin(); index != range.end(); index++)
			{
				std::set<size_t> attractorsToIgnore;
				if (m_sameData)
				{
					attractorsToIgnore.insert(index);
				}

				Solver::template solve<SubjectDataType, ActorDataType, ColliisionDataType>(
					attractorsToIgnore,
					attractantsData[index],
					attractantsBuffer[index],
					m_attractantsCollideData.getData()[index],
					m_attractors,
					m_attractorsCollideData
				);
			}
		}

	private:
		DoubleBuffVector<SubjectDataType>& m_attractants;
		DoubleBuffVector<ActorDataType>& m_attractors;
		DoubleBuffVector<ColliisionDataType>& m_attractantsCollideData;
		DoubleBuffVector<ColliisionDataType>& m_attractorsCollideData;
		bool m_sameData;
	};
	
// NBody solvers
	template<typename SubjectDataType, typename ActorDataType, typename Solver>
	class SolverParallelWrapper
	{
	public:
		SolverParallelWrapper(Solver::SolverParams params, DoubleBuffVector<SubjectDataType>& attractants, DoubleBuffVector<ActorDataType>& attractors, bool sameData) :
			m_params(params),
			m_attractants(attractants),
			m_attractors(attractors),
			m_sameData(sameData)
		{
		}

		void operator()(const oneapi::tbb::blocked_range<size_t>& range) const
		{
			const auto& attractantsData = m_attractants.getData();
			auto& attractantsBuffer = m_attractants.getBuffer();

			for (auto index = range.begin(); index != range.end(); index++)
			{
				std::set<size_t> attractorsToIgnore;
				if (m_sameData)
				{
					attractorsToIgnore.insert(index);
				}

				Solver::solve(attractorsToIgnore, attractantsData[index], attractantsBuffer[index], m_attractors, m_params);
			}
		}

	private:
		Solver::SolverParams m_params;
		DoubleBuffVector<SubjectDataType>& m_attractants;
		DoubleBuffVector<ActorDataType>& m_attractors;
		bool m_sameData;
	};
	
	class GravitySolver
	{
	public:
		template<typename DataType>
		static Eigen::Vector3d calcResultantGravityForce(const std::set<size_t>& elementsToSkip, const Eigen::Vector3d& targetPosition, DoubleBuffVector<DataType>& attractors, const double& gravitationalConstant)
		{
			auto resultantAcceleration = Eigen::Vector3d(0, 0, 0);

			auto& attractorsData = attractors.getData();

			for (size_t index = 0; index < attractorsData.size(); index++)
			{
				if (elementsToSkip.contains(index))
				{
					continue;
				}

				resultantAcceleration += calcGravityForce(targetPosition, attractorsData[index]);
			}

			return gravitationalConstant * resultantAcceleration;
		}

		template<typename DataType>
		static Eigen::Vector3d calcGravityForce(const Eigen::Vector3d& targetPosition, const DataType& attractor)
		{
			Eigen::Vector3d accelerationRadiusVector = attractor.position - targetPosition;
			double radiusVectorNorm = accelerationRadiusVector.norm();
			double accelerationMagnitude = attractor.mass / radiusVectorNorm / radiusVectorNorm;
			accelerationRadiusVector = accelerationRadiusVector * accelerationMagnitude / radiusVectorNorm;
			return accelerationRadiusVector;
		}
	};

	class EulerSolver : public GravitySolver
	{
	public:
		struct SolverParams
		{
			double gravitationalConstant;
			double stepSize;
		};

		template<typename TargetDataType, typename AttractorDataType>
		static void solve(const std::set<size_t>& elementsToSkip, const TargetDataType& targetData, TargetDataType& targetBuffer, DoubleBuffVector<AttractorDataType>& attractors, const SolverParams& params)
		{
			targetBuffer.position = targetData.position + params.stepSize * targetData.velocity;
			targetBuffer.velocity = targetData.velocity + params.stepSize * calcResultantGravityForce(elementsToSkip, targetData.position, attractors, params.gravitationalConstant);
		}
	};

	class EulerSolverPropulsed : public GravitySolver
	{
	public:
		struct SolverParams
		{
			double gravitationalConstant;
			double stepSize;
		};

		template<typename TargetDataType, typename AttractorDataType>
		static void solve(const std::set<size_t>& elementsToSkip, const TargetDataType& targetData, TargetDataType& targetBuffer, DoubleBuffVector<AttractorDataType>& attractors, const SolverParams& params)
		{
			targetBuffer.position = targetData.position + params.stepSize * targetData.velocity;
			targetBuffer.velocity = targetData.velocity + params.stepSize * (calcResultantGravityForce(elementsToSkip, targetData.position, attractors, params.gravitationalConstant) + targetData.propulsion);
		}
	};

	

	
}