#include "pch.h"

#pragma warning (disable: 4703)

#include "Core.h"
#include "Simulation.h"
#include "Graphics.h"

#include "SubscriptionManager.h"

#include "Prototypes.h"

#include <shared_mutex>
#include <chrono>
#include <thread>


using namespace projectSolar::Simulation;
using namespace projectSolar::Graphics;

constexpr double gravitationalConstant = 1.0;
constexpr size_t attractorsAmount = 15;
constexpr size_t attractantsAmount = 50;
constexpr double bigMass = 1e1;
constexpr double mediumMass = 1e-2;
constexpr double smallerMass = 1e-5;
constexpr double smallestMass = 1e-7;
constexpr double initSpeed = 5.0;
constexpr double initOrbit = 5.0;

void runnerDataSetup(SimulationRunner& runner);
void appRun();

using namespace projectSolar::Prototypes;

int main()
{
	LOG_INTT_CONSOLE("logs/log_sandbox.txt");
	LOG_DEBUG("[sandbox] Sandbox started");
	
	SubscriptionManager sub(2, 1 << 24);

	std::shared_ptr<TestEventHandler> test_0 = std::make_shared<TestEventHandler>(2, 1 << 24);
	std::shared_ptr<TestEventHandler> test_1 = std::make_shared<TestEventHandler>(2, 1 << 24);

	sub.subscribe(SubscriptionManager::TEST_MSG, test_0);
	sub.subscribe(SubscriptionManager::TEST_MSG, test_1);

	//std::this_thread::sleep_for(std::chrono::seconds(1));

	for (int i = 0; i < 100; i++)
	{
		SEND_EVENT(SubscriptionManager::TEST_MSG, &sub, 8, "Test msg");
	}

	//std::this_thread::sleep_for(std::chrono::seconds(1));

	LOG_DEBUG("[sandbox] Sandbox finished");
	return 0;
}

void appRun()
{
	auto simulation = std::make_shared<SimulationRunner>();
	auto ECS = std::make_shared<projectSolar::ECS::EntityManager>();

	runnerDataSetup(*simulation);

	auto app = std::make_unique<projectSolar::Application>(simulation, ECS);
	app->run();
}

void runnerDataSetup(SimulationRunner& runner)
{
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

		data.attractantsData.addElement({ rotation * ((index + 2) * radiusVector), rotation * (velocityVector / (index + 2)) * 2.0 });
	}

	Eigen::Vector3d nullVector(0.0, 0.0, 0.0);
	data.propulsedData.addElement({ radiusVector, velocityVector, nullVector });

	data.save("test_data");
}
