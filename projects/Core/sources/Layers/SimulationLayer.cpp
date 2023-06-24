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
}