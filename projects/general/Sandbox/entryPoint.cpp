#define PROFILING

#pragma warning (disable: 4703)

#include "Core.h"

#include "Logger.h"
#include "Profiler.h"

#define _USE_MATH_DEFINES

#include "SimulationRunner.h"

#include <iostream>
#include <array>
#include <math.h>
#include <string>
#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace projectSolar::Simulation;

constexpr double gravitationalConstant = 1.0;
constexpr size_t attractorsAmount = 15;
constexpr size_t attractantsAmount = 50;
constexpr double bigMass = 1e1;
constexpr double mediumMass = 1e-2;
constexpr double smallerMass = 1e-5;
constexpr double smallestMass = 1e-7;
constexpr double initSpeed = 5.0;
constexpr double initOrbit = 5.0;


int main()
{
	LOG_INTT_CONSOLE("logs/log_sandbox.txt");
	LOG_DEBUG("[sandbox] Sandbox started");
	PROFILE_BEGIN("sandbox");

	auto runner = SimulationRunner();
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
		Eigen::AngleAxisd rotation(angleForMedium * (double)index, rotationAxis);
		data.attractorsData.addElement({ mediumMass, rotation * radiusVector, rotation * velocityVector });
	}

	for (size_t index = 0; index < attractantsAmount; index++)
	{
		Eigen::AngleAxisd rotation(angleForSmall * (double)index, rotationAxis);

		data.attractantsData.addElement({ smallerMass, rotation * ((index + 2) * radiusVector), rotation * (velocityVector / (index + 2)) * 2.0 });
	}

	Eigen::Vector3d nullVector(0.0, 0.0, 0.0);
	data.propulsedData.addElement({ smallestMass, radiusVector, velocityVector, nullVector });

	data.save("test_data");

	auto* app = new projectSolar::Application(runner);
	app->run();
	delete(app);

	PROFILE_END();
	LOG_DEBUG("[sandbox] Sandbox finished");
	return 0;
}
