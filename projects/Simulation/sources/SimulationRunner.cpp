#include "pch.h"

#include "SimulationRunner.h"
#include "Serializer.h"
#include "Logger.h"

namespace projectSolar::Simulation
{
	SimulationRunner::SimulationRunner() :
		m_workersBarrier(m_concurrency + 1)
	{
		for (size_t i = 0; i < m_concurrency; i++)
		{
			m_workers.emplace_back(&SimulationRunner::worker, this, i);
		}
	}
	SimulationRunner::~SimulationRunner()
	{
		m_killThreads = true;
		m_workersBarrier.arrive_and_wait();
		m_workersBarrier.arrive_and_wait();
		for (size_t i = 0; i < m_workers.size(); i++)
		{
			m_workers[i].join();
		}
	}

	void SimulationRunner::run(const std::vector<std::shared_ptr<Simulation>>& simulations)
	{
		for (const auto& simulation : simulations)
		{
			distributeTasks(simulation->runParams(), simulation->task());
		}
		m_workersBarrier.arrive_and_wait();
		m_workersBarrier.arrive_and_wait();
	}
	void SimulationRunner::worker(uint8_t id)
	{
		Task task;
		while (!m_killThreads)
		{
			m_workersBarrier.arrive_and_wait();

			while (true)
			{
				{
					PROFILE_SCOPE("Worker acquiring task");
					std::unique_lock lock(m_queueMutex);
					if (m_taskQueue.empty())
					{
						lock.unlock();
						break;
					}

					task = m_taskQueue.front();
					m_taskQueue.pop();
					lock.unlock();
				}

				task.simulation->run(task);
			}
			
			m_workersBarrier.arrive_and_wait();
		}
	}
	void SimulationRunner::distributeTasks(const RunParams& params, std::vector<Task> order)
	{
		PROFILE_FUNCTION();
		
		size_t totalSize = 0;
		for (const Task& part : order)
		{
			totalSize += part.last - part.start + 1;
		}
		size_t taskSize = totalSize / params.granularity / (size_t)m_concurrency;
		if (taskSize < params.minimumSize)
		{
			taskSize = params.minimumSize;
		}

		for (Task& part : order)
		{
			while (part.size() >= taskSize)
			{
				m_taskQueue.emplace(part.simulation, part.start, part.start + taskSize - 1);
				part.start = part.start + taskSize;
			}

			if (part.size() > 0)
			{
				m_taskQueue.push(part);
			}
		}
	}
}
