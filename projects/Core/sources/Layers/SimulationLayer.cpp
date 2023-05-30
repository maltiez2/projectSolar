#include "pch.h"

#include "SimulationLayer.h"
#include "EventManagers/CommunicationManager.h"
#include "ECS/EntityComponentSystem.h"
#include "ECS/Components.h"


namespace projectSolar::Layers
{
	// Sim layer
	SimLayer::SimLayer(const Params& params) :
		SimulationStack(std::make_shared<Simulation::SimulationRunner>()),
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
			simulation->load(serializer);
		}
		serializer.close();
	}

	void SimLayer::process()
	{
		PROFILE_FUNCTION();
		
		std::unique_lock lock(m_dataMutex);
		
		auto targetStepsNumber = std::max((size_t)(m_params.simulationTimePerSecond / stepSize), 1ui64);
		
		m_lastStepsNumber = m_stepsDivider.onRunStart({ targetStepsNumber, m_params.timeRestriction });
		
		for (size_t stepNumber = 0; stepNumber < m_lastStepsNumber; stepNumber++)
		{
			step();
		}

		m_lastStepTime = m_stepsDivider.onRunEnd();

		EMIT_EVENT(SIMULATION_UPDATED, m_lastStepTime, m_lastStepsNumber, (float)(m_lastStepTime * stepSize));
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
		
		const size_t objectsNumber = m_objNumberInDebugLayout;
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

	void SimLayer::setObjNumberInDebugLayout(size_t number)
	{
		m_objNumberInDebugLayout = number;
	}
}