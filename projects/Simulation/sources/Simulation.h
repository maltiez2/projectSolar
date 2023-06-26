#pragma once

#include <queue>
#include <thread>
#include <shared_mutex>
#include <barrier>


namespace projectSolar::Simulation
{
	class SimulationData;
	class MotionSystem;
	
	/*
		Used to tell worker thread what system to process and on what data.
		Contains system id and parallelization parameters.
		Parameters have different meaning for different systems.
	*/
	struct Task
	{
		uint16_t system;
		uint64_t param_1;
		uint64_t param_2;
		uint64_t param_3;
	};

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

		struct Params
		{
			double stepSize;
			double gravConst;
		};

		explicit Simulation(Params params);
		~Simulation() = default;

		std::shared_ptr<SimulationData> data;
		std::shared_ptr<MotionSystem> motion;

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

		const uint32_t& getConcurency() const;

		void addToGeneralQueue(const Task& task);
		void addToThreadQueue(const uint32_t& thread, const Task& task);

	private:
		Simulation& m_simulation;

		const uint32_t m_concurrencyOffset = 1;

		uint32_t m_concurrency = std::thread::hardware_concurrency() - m_concurrencyOffset;
		std::mutex m_queueMutex;
		std::queue<Task> m_generalTaskQueue;
		std::vector<std::queue<Task>> m_threadsTaskQueues;
		std::vector<std::thread> m_workers;
		std::barrier<std::_No_completion_function> m_workersBarrier;
		bool m_killThreads = false;

		void worker(uint32_t id);
	};

	/*
		Manages system processing order and provides method for advancing simulation by step
		Manages double buffered data swapping
	*/
	class SimulationStepper
	{
	public:
		struct Params
		{
			uint32_t granularity;
		};

		explicit SimulationStepper(const Params& params, Simulation& simulation, ThreadPool& threadPool);
		~SimulationStepper() = default;

		void step();

		void setParams(const Params& params);
		const Params& getParams() const;

	private:
		Simulation& m_simulation;
		ThreadPool& m_threadPool;

		Params m_params;

		void distributeMotionTasks(const uint32_t& firtsWorkerId, const uint32_t& lastWorkerId, const uint32_t& granularity);
	};
}