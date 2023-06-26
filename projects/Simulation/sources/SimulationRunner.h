#pragma once

#include <memory>
#include <chrono>
#include <shared_mutex>
#include <semaphore>

namespace projectSolar::Simulation
{
	class Simulation;
	class ThreadPool;
	class SimulationStepper;
	class SimulationData;

	/*
		Runs simulation with given target TPS, or advances it by given steps number.
		Provides API for Core to interact with simulation (get and set data, set parameters).
	*/
	class SimulationRunner
	{
	public:
		struct Params
		{
			double stepSize;
			double gravConst;
			uint32_t granularity;
		};

		enum class State
		{
			IDLE,
			RUN,
			ADVANCE
		};

		const uint32_t defaultTicksPerSecond = 1;

		explicit SimulationRunner(const Params& params);
		~SimulationRunner();

		void start();
		void stop();
		void advance(uint32_t ticks);

		void setTicksPerSecond(uint32_t tickPerSecond);
		void setGranularity(uint32_t granularity);
		
		double getTicksPerSecond() const;
		uint32_t getTicksElapsed() const;
		State getState();

		std::shared_ptr<SimulationData> getData();

	private:
		std::shared_ptr<Simulation> m_simulation;
		std::shared_ptr<ThreadPool> m_threadPool;
		std::shared_ptr<SimulationStepper> m_stepper;

		std::binary_semaphore m_workerSemaphore = std::binary_semaphore(0);
		std::thread m_worker;
		std::chrono::system_clock::time_point m_stepStart;
		std::chrono::system_clock::time_point m_stepFinish;

		uint32_t m_ticksPackSize = 1;
		uint32_t m_targetTicksPerSecond = 0;
		double m_microsecondsPerTick = 0.0;
		double m_ticksPerSecond = 0.0;
		uint32_t m_ticksElapsed = 0;
		
		std::shared_mutex m_runParamsMutex;
		uint32_t m_ticksToAdvance = 0;
		State m_state = State::IDLE;
		bool m_killWorker = false;

		void worker();
		void runImpl();
		void advanceImpl();
		void wait();
	};
}