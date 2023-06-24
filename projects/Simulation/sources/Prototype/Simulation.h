#pragma once

#include "SimulationData.h"
#include "SimulationSystems.h"

namespace projectSolar::Simulation
{
	/*
		Stores simulation data and systems
		Provides function for workers thread to branch into processing system stated in task
	*/
	class Simulation
	{
	public:
		enum : uint16_t
		{
			MOTION
		};

		SimulationData data;

		MotionSystem motion;

		void processTask(Task task);
	};

	/*
		Manages worker threads and tasks queues
		Have two queues for predefined distribution of tasks, and for runtime distribution
	*/
	class ThreadPool
	{
	public:
		explicit ThreadPool(Simulation& simulation);
		~ThreadPool();

		void processTasks();

		size_t getConcurency();

		void addToGeneralQueue(Task task);
		void addToThreadQueue(size_t thread, Task task);

	private:
		Simulation& m_simulation;

		void worker(size_t id);
	};

	/*
		Manages system processing order and provides method for advancing simulation by step
		Manages double buffered data swapping
	*/
	class SimulationStepper
	{
	public:
		explicit SimulationStepper(Simulation& simulation, ThreadPool& threadPool);
		~SimulationStepper();

		void step();

	private:
		Simulation& m_simulation;
		ThreadPool& m_threadPool;
	};
}