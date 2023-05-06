#pragma once

#include "SimulationCommon.h"
#include "DoubleBufferedContainer.h"

namespace projectSolar::Simulation
{
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

	template<typename AttractantDataType, typename AttractorDataType, typename Solver>
	class SolverParallelWrapper
	{
	public:
		SolverParallelWrapper(Solver::SolverParams params, DoubleBuffVector<AttractantDataType>& attractants, DoubleBuffVector<AttractorDataType>& attractors) :
			m_params(params),
			m_attractants(attractants),
			m_attractors(attractors)
		{
		}

		void operator()(const oneapi::tbb::blocked_range<size_t>& range) const
		{			
			const auto& attractantsData = m_attractants.getData();
			auto& attractantsBuffer = m_attractants.getBuffer();

			for (auto index = range.begin(); index != range.end(); index++)
			{
				Solver::solve(std::set<size_t>{index}, attractantsData[index], attractantsBuffer[index], m_attractors, m_params);
			}
		}

	private:
		Solver::SolverParams m_params;
		DoubleBuffVector<AttractantDataType>& m_attractants;
		DoubleBuffVector<AttractorDataType>& m_attractors;
	};

	
}