#include "pch.h"

#include "SimulationRunner.h"
#include "Simulation.h"


namespace projectSolar::Simulation
{
	SimulationRunner::SimulationRunner(const Params& params) :
		m_worker(&SimulationRunner::worker, this)
	{
		m_simulation = std::make_shared<Simulation>(Simulation::Params(params.stepSize, params.gravConst));
		m_threadPool = std::make_shared<ThreadPool>(*m_simulation);
		m_stepper = std::make_shared<SimulationStepper>(SimulationStepper::Params(params.granularity), *m_simulation, *m_threadPool);
	}
	SimulationRunner::~SimulationRunner()
	{
		m_killWorker = false;
		m_workerSemaphore.release();
		m_worker.join();
	}
	
	void SimulationRunner::start()
	{
		std::unique_lock lock(m_runParamsMutex);
		if (m_targetTicksPerSecond == 0)
		{
			m_targetTicksPerSecond = defaultTicksPerSecond;
		}
		m_state = State::RUN;
		m_workerSemaphore.release();
	}
	void SimulationRunner::stop()
	{
		std::unique_lock lock(m_runParamsMutex);
		m_ticksToAdvance = 0;
		m_state = State::IDLE;
	}
	void SimulationRunner::advance(uint32_t ticks)
	{
		std::unique_lock lock(m_runParamsMutex);
		m_ticksToAdvance += ticks;
		m_state = State::ADVANCE;
	}

	void SimulationRunner::worker()
	{
		while (!m_killWorker)
		{
			std::shared_lock rLock(m_runParamsMutex);
			switch (m_state)
			{
				using enum State;

			case IDLE:
				rLock.unlock();
				m_workerSemaphore.acquire();
				break;
			
			case RUN:
				rLock.unlock();
				runImpl();
				break;
			
			case ADVANCE:
				rLock.unlock();
				advanceImpl();
				
				std::unique_lock wLock(m_runParamsMutex);
				m_ticksToAdvance -= m_ticksPackSize;
				if (m_ticksToAdvance <= 0 && m_state == State::ADVANCE)
				{
					m_state = State::IDLE;
					m_ticksToAdvance = 0;
				}
				wLock.unlock();
				break;
			}
		}
	}
	void SimulationRunner::runImpl()
	{
		m_stepStart = std::chrono::system_clock::now();
		
		for (uint32_t tick = 0; tick <= m_ticksPackSize; tick++)
		{
			m_stepper->step();
		}

		m_ticksElapsed += m_ticksPackSize;

		m_stepFinish = std::chrono::system_clock::now();

		wait();
	}
	void SimulationRunner::advanceImpl()
	{
		for (uint32_t tick = 0; tick <= m_ticksPackSize; tick++)
		{
			m_stepper->step();
		}
		m_ticksElapsed += m_ticksPackSize;
	}
	void SimulationRunner::wait()
	{
		std::chrono::duration<double, std::micro> stepDuration = m_stepFinish - m_stepStart;
		m_ticksPerSecond = (double)m_ticksPackSize * 1e6 / stepDuration.count();
		
		std::shared_lock lock(m_runParamsMutex);
		if (stepDuration.count() < m_microsecondsPerTick)
		{
			lock.unlock();

			std::chrono::duration<double, std::micro> delta(m_microsecondsPerTick - stepDuration.count());
			auto deltaDuration = std::chrono::duration_cast<std::chrono::microseconds>(delta);
			std::this_thread::sleep_for(std::chrono::microseconds(deltaDuration.count()));
		}
	}

	void SimulationRunner::setTicksPerSecond(uint32_t tickPerSecond)
	{
		m_targetTicksPerSecond = tickPerSecond;
		m_microsecondsPerTick = 1e6 / (double)tickPerSecond;
	}
	void SimulationRunner::setGranularity(uint32_t granularity) // @TODO Add lock to protect in case if multiple parameters
	{
		auto params = m_stepper->getParams(); // Redundant for now
		params.granularity = granularity;
		m_stepper->setParams(params);
	}

	double SimulationRunner::getTicksPerSecond() const
	{
		return m_ticksPerSecond;
	}
	uint32_t SimulationRunner::getTicksElapsed() const
	{
		return m_ticksElapsed;
	}
	SimulationRunner::State SimulationRunner::getState()
	{
		std::shared_lock lock(m_runParamsMutex);
		return m_state;
	}

	std::shared_ptr<SimulationData> SimulationRunner::getData()
	{
		return m_simulation->data;
	}
}