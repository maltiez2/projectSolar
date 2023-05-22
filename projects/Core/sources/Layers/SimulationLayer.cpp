#include "pch.h"

#include "SimulationLayer.h"
#include "EventManagers/CommunicationManager.h"
#include "ECS/EntityComponentSystem.h"
#include "ECS/Components.h"


namespace projectSolar::Layers
{
	// Sim layer
	SimLayer::SimLayer(const Params& params) :
		m_params(params)
	{

	}

	void SimLayer::save(const std::string& filePath)
	{
		std::shared_lock lock(m_dataMutex);

		Simulation::Serializer serializer;
		errno_t error = serializer.open<Simulation::Serializer::fileMode::writeBytes>(filePath);
		if (error != 0)
		{
			LOG_ERROR("[SimLayer] Cannot open file for writing '", filePath, "', error: ", error);
			return;
		}
		for (const auto& [id, simulation] : m_registered)
		{
			serializer.serialize(m_simOrders[id]);
			simulation->save(serializer);
		}
		serializer.close();
	}
	void SimLayer::load(const std::string& filePath)
	{
		std::unique_lock lock(m_dataMutex);

		Simulation::Serializer serializer;
		errno_t error = serializer.open<Simulation::Serializer::fileMode::readBytes>(filePath);
		if (error != 0)
		{
			LOG_ERROR("[SimLayer] Cannot open file for reading '", filePath, "', error: ", error);
			return;
		}
		for (const auto& [id, simulation] : m_registered)
		{
			m_simOrders[id] = {};
			serializer.deserialize(m_simOrders[id]);
			simulation->load(serializer);
		}
		serializer.close();
	}

	void SimLayer::process()
	{
		std::unique_lock lock(m_dataMutex);
		
		m_lastStepsNumber = m_stepsDivider.onRunStart({ 16, m_params.timeRestriction });
		double stepSize = m_params.simulationTimePerSecond * m_secondsPerFrame / (double)m_lastStepsNumber;

		for (size_t step = 0; step < m_lastStepsNumber; step++)
		{
			for (const auto& [id, simulation] : m_attached)
			{
				LOG_ASSERT(m_simOrders.contains(id), "Add orders for simulation: ", id);
				simulation->stepSize = stepSize;
				m_runner.run({ 5, 1 }, simulation, m_simOrders[id]);
			}

			for (const auto& [id, simulation] : m_attached)
			{
				simulation->swapData();
			}
		}

		m_lastStepTime = m_stepsDivider.onRunEnd();

		EMIT_EVENT(SIMULATION_UPDATED, m_lastStepTime, m_lastStepsNumber);
	}
	void SimLayer::onEvent(Graphics::InputEvent* ev)
	{
		// No events to process
	}
	void SimLayer::generateDebugLayout(size_t motionId, size_t gravityId)
	{
		std::unique_lock lock(m_dataMutex);
		
		auto simulation = m_attached[motionId];
		auto motionSim = std::dynamic_pointer_cast<Simulation::Motion>(simulation);
		auto& data = motionSim->data;
		data.clear();

		auto ECS = Com::get().ECS;
		auto view = ECS->getView<Components::Dynamic>();
		for (auto& entity : view)
		{
			ECS->destroy(entity);
		}
		
		const size_t objectsNumber = 100;
		const double bigMass = 1e0;
		const double smallMass = 1e-3;
		const double initOrbit = 1.0;
		const double initSpeed = 1.0;

		double angle = 2 * std::numbers::pi / (double)objectsNumber;
		Eigen::Vector3d radiusVector{ initOrbit, 0, 0 };
		Eigen::Vector3d velocityVector{ 0, initSpeed, 0 };
		Eigen::Vector3d rotationAxis{ 0, 0, 1 };
		Eigen::Vector3d nullVector(0.0, 0.0, 0.0);
		
		data.reserve(objectsNumber + 2);

		data.addElement({ bigMass, nullVector, nullVector, nullVector });
		auto centralObj = ECS->create();
		ECS->insert<Components::CelestialObject>(centralObj, Components::LongTitle{ "Central" });
		ECS->insert<Components::Dynamic>(centralObj, 0ui64);
		ECS->insert<Components::MapObject>(centralObj, 0ui32, 1.0f, 1.0f, 0.0f, 1.0f);

		data.addElement({ smallMass, 4.0 * radiusVector, 0.5 * velocityVector, nullVector });
		auto redObj = ECS->create();
		ECS->insert<Components::CelestialObject>(redObj, Components::LongTitle{ "Red" });
		ECS->insert<Components::Dynamic>(redObj, 1ui64);
		ECS->insert<Components::MapObject>(redObj, 1ui32, 1.0f, 0.0f, 0.0f, 1.0f);
		ECS->insert<Components::Player>(redObj);

		for (size_t index = 0; index < objectsNumber; index++)
		{
			Eigen::AngleAxisd rotation(angle * (double)index, rotationAxis);
			data.addElement({ smallMass, rotation * radiusVector, rotation * velocityVector, nullVector });
			auto justObj = ECS->create();
			ECS->insert<Components::Dynamic>(justObj, index + 2);
			ECS->insert<Components::MapObject>(justObj, (uint32_t)index, 1.0f, 1.0f, 1.0f, 1.0f);
		}

		setSimOrder(motionId, { {1, objectsNumber + 1} });
		setSimOrder(gravityId, { {1, objectsNumber + 1} });

		EMIT_EVENT(SIMULATION_UPDATED);
	}

	size_t SimLayer::getLastStepsNumber() const
	{
		return m_lastStepsNumber;
	}
	float SimLayer::getLastStepTime() const
	{
		return m_lastStepTime;
	}
	std::vector<Simulation::Task>& SimLayer::getOrder(size_t id)
	{
		LOG_ASSERT(m_simOrders.contains(id), "[SimLayer] Simulation with id '", id, "' has now orders assigned")
		return m_simOrders[id];
	}
	void SimLayer::setTimeRest(double timeRestrictionSeconds)
	{
		m_params.timeRestriction = (float)timeRestrictionSeconds;
	}
	void SimLayer::setTimePerSecond(double simulationTimePerSecond)
	{
		m_params.simulationTimePerSecond = simulationTimePerSecond;
	}
	void SimLayer::setSecondsPerFrame(double secondsPerFrame)
	{
		m_secondsPerFrame = secondsPerFrame;
	}
	void SimLayer::setSimOrder(size_t id, const std::vector<Simulation::Task>& order)
	{
		m_simOrders[id] = order;
	}
	void SimLayer::addToSimOrder(size_t id, const Simulation::Task& task)
	{
		auto& order = m_simOrders[id];
		m_simOrders[id].push_back(task);
		
	}
	void SimLayer::excludeFromSimOrder(size_t id, const Simulation::Task& task)
	{
		for (auto it = m_simOrders[id].begin(); it != m_simOrders[id].end(); ++it)
		{
			if (task.start == (*it).start && task.last == (*it).last)
			{
				m_simOrders[id].erase(it);
				break;
			}
		}
	}

	// Steps divider
	size_t StepsDivider::onRunStart(const Params& params)
	{
		m_startTimepoint = std::chrono::steady_clock::now();
		m_params = params;

		if (m_results.empty())
		{
			m_currentStepNumber = m_params.desiredStepsNumber;
			return m_currentStepNumber;
		}

		StepsDivider::StepData average(0.0f, 0.0f, 0.0f, 0.0f);
		for (const auto& result : m_results)
		{
			average.excessTime += result.excessTime;
			average.secondsPerStep += result.secondsPerStep;
			average.stepsNumber += result.stepsNumber;
			average.timeRestrictionSeconds += result.timeRestrictionSeconds;
		}
		average.excessTime /= (float)m_results.size();
		average.secondsPerStep /= (float)m_results.size();
		average.stepsNumber /= m_results.size();
		average.timeRestrictionSeconds /= (float)m_results.size();

		float excessTime = average.excessTime + average.timeRestrictionSeconds - m_params.timeRestrictionSeconds;
		float excessSteps = excessTime / average.secondsPerStep;
		excessSteps = excessSteps + 0.1f * std::abs(excessSteps);
		int64_t newStepsNumber = (10 * (int64_t)average.stepsNumber - (int64_t)(excessSteps * 10.0f)) / 10;

		m_currentStepNumber = std::min((uint64_t)std::max(newStepsNumber, 1i64), m_params.desiredStepsNumber);
		return m_currentStepNumber;
	}
	float StepsDivider::onRunEnd()
	{
		auto endTimepoint = std::chrono::steady_clock::now();
		uint64_t start = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_startTimepoint).time_since_epoch().count();
		uint64_t end = std::chrono::time_point_cast<std::chrono::nanoseconds>(endTimepoint).time_since_epoch().count();

		float totalTimeSeconds = (float)(end - start) * 1e-9f;
		float excessTimeSeconds = totalTimeSeconds - m_params.timeRestrictionSeconds;
		float secondsPerStep = totalTimeSeconds / (float)m_currentStepNumber;

		if (m_results.size() > m_queieSize)
		{
			m_results.pop_front();
		}

		m_results.emplace_back(
			m_currentStepNumber,
			m_params.timeRestrictionSeconds,
			secondsPerStep,
			excessTimeSeconds
		);

		return secondsPerStep;
	}
}