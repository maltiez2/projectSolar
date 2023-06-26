#include "pch.h"

#include "Simulation.h"
#include "SimulationSystems.h"
#include "SimulationData.h"


namespace projectSolar::Simulation
{
	// SimulationStepper
	SimulationStepper::SimulationStepper(const Params& params, Simulation& simulation, ThreadPool& threadPool) :
		m_simulation(simulation),
		m_threadPool(threadPool),
		m_params(params)
	{
	}

	void SimulationStepper::step()
	{
		PROFILE_FUNCTION();
		
		const uint32_t& concurrency = m_threadPool.getConcurency();
		const uint32_t& granularity = m_params.granularity;

		distributeMotionTasks(0, concurrency - 1, granularity);
		m_threadPool.processTasks();
	}
	void SimulationStepper::distributeMotionTasks(const uint32_t& firtsWorkerId, const uint32_t& lastWorkerId, const uint32_t& granularity)
	{
		PROFILE_FUNCTION();
		
		const uint32_t concurrency = lastWorkerId - firtsWorkerId + 1;

		const auto& celestialObjects = m_simulation.data->celestialObjects.celestialObjects;

		// ALL OBJECT CLASSES EXCEPT SHORT_LIVED_OBJECTS
		for (size_t objectClass = 0; objectClass < ClestialObjectsData::SHORT_LIVED_OBJECTS; objectClass++)
		{
			const size_t objectsAmount = celestialObjects[objectClass].size();

			if (objectsAmount == 0)
			{
				continue;
			}

			if (objectsAmount < (size_t)concurrency * concurrency)
			{
				m_threadPool.addToGeneralQueue({ Simulation::MOTION, objectClass, 0, objectsAmount - 1 });
				continue;
			}

			const size_t taskSize = objectsAmount / concurrency;
			for (uint32_t taskIndex = 0; taskIndex <= lastWorkerId - firtsWorkerId; taskIndex++)
			{
				m_threadPool.addToThreadQueue(
					taskIndex + firtsWorkerId,
					{
						Simulation::MOTION,
						objectClass,
						taskIndex * taskSize,
						(taskIndex + 1) * taskSize - 1
					}
				);
			}
			if (objectsAmount % concurrency != 0)
			{
				m_threadPool.addToGeneralQueue(
					{
						Simulation::MOTION,
						objectClass,
						concurrency * taskSize,
						objectsAmount - 1
					}
				);
			}
		}

		// SHORT_LIVED_OBJECTS
		// Using general queue due to possible big amount of invalid objects, that is caused by frequent object deletion
		const size_t objectsAmount = celestialObjects[ClestialObjectsData::SHORT_LIVED_OBJECTS].size();
		if (objectsAmount == 0)
		{
			return;
		}

		const size_t taskAmount = (size_t)concurrency * granularity;
		const size_t taskSize = objectsAmount / taskAmount;

		for (size_t taskIndex = 0; taskIndex < taskAmount; taskIndex++)
		{
			m_threadPool.addToGeneralQueue(
				{
					Simulation::MOTION,
					ClestialObjectsData::SHORT_LIVED_OBJECTS,
					taskIndex * taskSize,
					(taskIndex + 1) * taskSize - 1
				}
			);
		}
		if (objectsAmount % taskAmount != 0)
		{
			m_threadPool.addToGeneralQueue(
				{
					Simulation::MOTION,
					ClestialObjectsData::SHORT_LIVED_OBJECTS,
					taskAmount * taskSize,
					objectsAmount - 1
				}
			);
		}
	}
	
	void SimulationStepper::setParams(const Params& params)
	{
		m_params = params;
	}
	const SimulationStepper::Params& SimulationStepper::getParams() const
	{
		return m_params;
	}

	// Simulation
	Simulation::Simulation(Params params)
	{
		data = std::make_shared<SimulationData>();
		motion = std::make_shared<MotionSystem>(MotionSystem::Params(params.stepSize, params.gravConst), *data);
	}
	
	void Simulation::processTask(Task task)
	{
		switch (task.system)
		{
		case MOTION:
			motion->solve(task);
			break;
		default:
			LOG_ASSERT(false, " [Simulation] Unknown system id: ", task.system);
		}
	}

	// ThreadPool
	ThreadPool::ThreadPool(Simulation& simulation) :
		m_simulation(simulation),
		m_workersBarrier(m_concurrency + 1)
	{
		m_threadsTaskQueues.resize(m_concurrency);
		
		for (uint32_t i = 0; i < m_concurrency; i++)
		{
			m_workers.emplace_back(&ThreadPool::worker, this, i);
		}
	}
	ThreadPool::~ThreadPool()
	{
		m_killThreads = true;
		m_workersBarrier.arrive_and_wait();
		m_workersBarrier.arrive_and_wait();
		for (size_t i = 0; i < m_workers.size(); i++)
		{
			m_workers[i].join();
		}
	}
	void ThreadPool::processTasks()
	{
		m_workersBarrier.arrive_and_wait(); // Release workers
		m_workersBarrier.arrive_and_wait(); // Wait workers to finish
	}
	void ThreadPool::worker(uint32_t id)
	{
		Task task;
		while (!m_killThreads)
		{
			m_workersBarrier.arrive_and_wait(); // Wait to be released

			while (!m_threadsTaskQueues[id].empty())
			{
				m_simulation.processTask(m_threadsTaskQueues[id].front());
				m_threadsTaskQueues[id].pop();
			}
			
			while (true)
			{
				std::unique_lock lock(m_queueMutex);
				if (m_generalTaskQueue.empty())
				{
					lock.unlock();
					break;
				}

				task = m_generalTaskQueue.front();
				m_generalTaskQueue.pop();
				lock.unlock();

				m_simulation.processTask(task);
			}

			m_workersBarrier.arrive_and_wait(); // Release master
		}
	}

	const uint32_t& ThreadPool::getConcurency() const
	{
		return m_concurrency;
	}
	void ThreadPool::addToGeneralQueue(const Task& task)
	{
		m_generalTaskQueue.push(task);
	}
	void ThreadPool::addToThreadQueue(const uint32_t& thread, const Task& task)
	{
		LOG_ASSERT(thread <= m_concurrency, " [ThreadPool] Wrong thread id: ", thread);
		
		m_threadsTaskQueues[thread].push(task);
	}
}
