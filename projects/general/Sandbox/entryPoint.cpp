#define PROFILING
#define DEBUG_LOGGING

#pragma warning (disable: 4703)

#include "Logger.h"
#include "Profiler.h"
#include "SimulationRunner.h"
#include "Serialization.h"

#define _USE_MATH_DEFINES

#include <iostream>
#include <array>
#include <math.h>
#include <string>

#include <Eigen/Core>
#include <Eigen/Geometry>

#define quote(x) #x

using namespace projectSolar::Simulation;

constexpr double gravitationalConstant = 1.0;
constexpr size_t attractorsAmount = 1e2;
constexpr size_t attractantsAmount = 1e4;
constexpr double bigMass = 1e0;
constexpr double mediumMass = 1e-2;
constexpr double smallerMass = 1e-5;
constexpr double initSpeed = 1.0;
constexpr double initOrbit = 1.0;
constexpr uint32_t stepsNumber = 6e2;
constexpr double stepSize = 1e-2;
constexpr uint32_t stepsToLog = 1e1;
constexpr uint32_t objectsToLog = 10;

void runSimulationTest();

int main()
{
	LOG_INTT_CONSOLE("logs/log_sandbox.txt");
	LOG_DEBUG("[sandbox] Sandbox started");
	PROFILE_BEGIN("sandbox");

	

	PROFILE_END();
	LOG_DEBUG("[sandbox] Sandbox finished");
	return 0;
}

void runSimulationTest()
{
	PROFILE_FUNCTION();
	
	auto runner = SimulationRunner();

	// GENERATING INITIAL DATA

	auto& data = runner.getData();

	double angleForSmall = 2 * M_PI / (double)attractantsAmount;
	double angleForMedium = 2 * M_PI / (double)attractorsAmount;
	Eigen::Vector3d radiusVector{ initOrbit, 0, 0 };
	Eigen::Vector3d velocityVector{ 0, initSpeed, 0 };
	Eigen::Vector3d rotationAxis{ 0, 0, 1 };

	data.attractorsData.reserve(attractorsAmount);
	data.attractantsData.reserve(attractantsAmount);

	data.attractorsData.addElement({ bigMass, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 } });

	for (size_t index = 1; index < attractorsAmount; index++)
	{
		Eigen::AngleAxisd rotation(angleForMedium * (double)(index - 1), rotationAxis);
		data.attractorsData.addElement({ mediumMass, rotation * radiusVector, rotation * velocityVector });

		if (index % 10 == 0)
		{
			LOG_DEBUG("[sandbox] Adding attractor: " + std::to_string(index));
		}
	}

	for (size_t index = 0; index < attractantsAmount; index++)
	{
		Eigen::AngleAxisd rotation(angleForSmall * (double)index, rotationAxis);

		data.attractantsData.addElement({ smallerMass, rotation * ((index + 2) * radiusVector), rotation * (velocityVector / (index + 2)) });

		if (index % 100 == 0)
		{
			LOG_DEBUG("[sandbox] Adding attractant: " + std::to_string(index));
		}
	}

	{
		PROFILE_SCOPE("save");
		data.save("data_ser_test");
	}

	{
		PROFILE_SCOPE("load");
		data.load("data_ser_test");
	}

	

	// RUNNING SIMULATION
	for (uint32_t step = 0; step < stepsNumber; step++)
	{
		PROFILE_SCOPE("step");
		double secondsElapsed = runner.run({ gravitationalConstant, stepSize, 0.9f, 60, 10, -0.1f });
		LOG_INFO("[sandbox] Frame: " + std::to_string(step) + ", duration in seconds: " + std::to_string(secondsElapsed) + ", apparent FPS: " + std::to_string(1 / secondsElapsed * 0.8));
	}
}
