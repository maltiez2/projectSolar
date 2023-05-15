#pragma once

#include <shared_mutex>
#include <barrier>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>

namespace projectSolar::Simulation
{	
	class Serializer;
	
	struct Task
	{
		size_t start;
		size_t last;

		size_t size() const;
	};

	struct RunParams
	{
		size_t granularity;
		size_t minimumSize;
	};

	class Simulation
	{
	public:
		Simulation() = default;
		virtual ~Simulation() = default;

		virtual void run(Task task) = 0;
		virtual void swapData() = 0;
		virtual void save(Serializer& serializer) = 0;
		virtual void load(Serializer& serializer) = 0;

		double stepSize = 1.0;
	};
	
	class SimulationRunner
	{
	public:
		SimulationRunner();
		~SimulationRunner();

		void run(const RunParams& params, std::shared_ptr<Simulation> simulation, const std::vector<Task>& order);

	private:
		uint8_t m_concurrency = (uint8_t)std::thread::hardware_concurrency();
		std::shared_ptr<Simulation> m_currentSimulation;
		std::vector<std::thread> m_workers;
		std::barrier<std::_No_completion_function> m_workersBarrier;
		std::queue<Task> m_taskQueue;
		std::mutex m_queueMutex;
		bool m_killThreads = false;

		void worker(uint8_t id);
		void distributeTasks(const RunParams& params, std::vector<Task> order);
	};
}
