#pragma once

#include "Simulation.h"

#include <shared_mutex>
#include <barrier>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <vector>

namespace projectSolar::Simulation
{
	class SimulationRunner
	{
	public:
		SimulationRunner(std::shared_ptr<Simulation> simulation);
		~SimulationRunner();

		void run(const std::vector<size_t>& types);

	private:
		std::shared_ptr<Simulation> m_simulation;
		
		uint8_t m_concurrency = (uint8_t)std::thread::hardware_concurrency();
		std::vector<std::thread> m_workers;
		std::barrier<std::_No_completion_function> m_workersBarrier;
		std::queue<Task> m_taskQueue;
		std::mutex m_queueMutex;
		bool m_killThreads = false;

		void worker(uint8_t id);
		void distributeTasks(const RunParams& params, std::vector<Task> order);
	};
}
