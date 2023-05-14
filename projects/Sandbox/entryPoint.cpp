#include "pch.h"

#pragma warning (disable: 4703)

#include "Core.h"
#include "Simulation.h"
#include "Graphics.h"

#include <shared_mutex>
#include <chrono>
#include <thread>
#include <cmath>


using namespace projectSolar;

void setupData(Simulation::DoubleBuffVector<Simulation::Motion::Data>& data);
void appRun();

int main()
{
	LOG_INTT_CONSOLE("logs/log_sandbox.txt");
	LOG_DEBUG("[sandbox] Sandbox started");
	
	appRun();

	LOG_DEBUG("[sandbox] Sandbox finished");
	return 0;
}

void appRun()
{
	auto ECS = std::make_shared<ECS::EntityManager>();

	auto app = std::make_unique<Application>(ECS);
	Simulation::DoubleBuffVector<Simulation::Motion::Data>& data = app->DEBUG_getSimData();
	setupData(data);
	app->run();
}

void setupData(Simulation::DoubleBuffVector<Simulation::Motion::Data>& data)
{
	const size_t objectsNumber = 36;
	const double bigMass = 1e0;
	const double smallMass = 1e-3;
	const double initOrbit = 1.0;
	const double initSpeed = 1.0;

	double angle = 2 * M_PI / (double)objectsNumber;
	Eigen::Vector3d radiusVector{ initOrbit, 0, 0 };
	Eigen::Vector3d velocityVector{ 0, initSpeed, 0 };
	Eigen::Vector3d rotationAxis{ 0, 0, 1 };
	Eigen::Vector3d nullVector(0.0, 0.0, 0.0);
	
	data.reserve(objectsNumber + 2);
	data.addElement({ bigMass, nullVector, nullVector, nullVector });
	data.addElement({ smallMass, 4.0 * radiusVector, 0.5 * velocityVector, nullVector });

	for (size_t index = 0; index < objectsNumber; index++)
	{
		Eigen::AngleAxisd rotation(angle * (double)index, rotationAxis);
		data.addElement({ smallMass, rotation * radiusVector, rotation * velocityVector, nullVector });
	}

}
