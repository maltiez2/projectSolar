#include "pch.h"

#include "Motion.h"
#include "Gravity.h"
#include "Serializer.h"
#include "Logger.h"


namespace projectSolar::Simulation
{
	size_t Task::size() const
	{
		if (start > last)
		{
			return 0;
		}

		return last - start + 1;
	}
	
	Motion::Motion(Params params, RunParams concurrencyParams) :
		params(params),
		runParams(concurrencyParams)
	{
		LOG_ASSERT(params.precesision / params.stepSize > 0.5, "[Motion] Insufficient step precision: ", params.precesision / params.stepSize);
		unskipStep = std::max((uint16_t)(params.precesision / params.stepSize), runParams.runEachXStep);
		stepSize = params.stepSize * (double)unskipStep;
	}
	void Motion::prepare()
	{
		auto& buffer = data.getBuffer();
		for (auto& element : buffer)
		{
			element.force = { 0.0, 0.0, 0.0 };
		}
	}
	void Motion::run(Task task)
	{
		PROFILE_FUNCTION();

		auto& initial = data.getData();
		auto& result = data.getBuffer();
		
		for (size_t index = task.start; index <= task.last; index++)
		{
			const auto& initialData = initial[index];
			auto& resultData = result[index];
			
			resultData.position = initialData.position + stepSize * initialData.velocity;
			resultData.velocity = initialData.velocity + stepSize * initialData.force / initialData.mass;
		}
	}
	void Motion::swapData()
	{
		data.swap();
	}
	void Motion::save(Serializer& serializer)
	{
		serializer.serialize(data);
	}
	void Motion::load(Serializer& serializer)
	{
		serializer.deserialize(data);
	}
	bool Motion::skip(const uint16_t& step)
	{
		return step % unskipStep != 0;
	}
	std::vector<Task> Motion::task()
	{
		return { {this, 0, data.size() - 1}};
	}
	void Motion::setRunParams(RunParams concurencyParams)
	{
		runParams = concurencyParams;
	}
	RunParams Motion::getRunParams()
	{
		return { runParams.granularity, std::max(data.size() / runParams.granularity, 1ui64) };
	}
	uint8_t Motion::getGroup()
	{
		return PRIMARY_GROUP;
	}


	Gravity::Gravity(Params params, RunParams concurrencyParams, Motion::MotionData* motionData) :
		objects(motionData),
		skipSameIndex(0),
		params(params),
		runParams(concurrencyParams)
	{
		attractors.emplace_back(motionData);
	}
	Gravity::Gravity(Params params, RunParams concurrencyParams, Motion::MotionData* initObjects, std::vector<Motion::MotionData*> initAttractors) :
		objects(initObjects),
		params(params),
		runParams(concurrencyParams)
	{
		for (Motion::MotionData* attractorsData : initAttractors)
		{
			if (attractorsData == initObjects)
			{
				LOG_ASSERT(skipSameIndex == -1, "Same motion data passed to gravity simulation twice as attractors data");
				skipSameIndex = attractors.size();
			}
			
			attractors.emplace_back(attractorsData);
		}
	}
	void Gravity::prepare()
	{
		// Nothing to prepare
	}
	void Gravity::run(Task task)
	{
		PROFILE_FUNCTION();
		
		auto& initial = objects->getData();
		auto& result = objects->getBuffer();

		for (size_t index = task.start; index <= task.last; index++)
		{
			for (size_t attrIndex = 0; attrIndex < attractors.size(); attrIndex++)
			{
				auto& attractorsData = attractors[attrIndex]->getData();

				for (size_t objIndex = 0; objIndex < attractorsData.size(); objIndex++)
				{
					if (!attractors[attrIndex]->isValid(objIndex))
					{
						continue;
					}
					if (skipSameIndex == attrIndex && objIndex == index)
					{
						continue;
					}

					Eigen::Vector3d direction = attractorsData[objIndex].position - initial[index].position;
					double distance = direction.norm();
					result[index].force += direction * attractorsData[objIndex].mass * initial[index].mass / distance / distance / distance * params.gravConst;
				}
			}
		}
	}
	void Gravity::swapData()
	{
		// Not responsible for swapping
	}
	void Gravity::save(Serializer& serializer)
	{
		// Nothing to serialize
	}
	void Gravity::load(Serializer& serializer)
	{
		// Nothing to deserialize
	}
	bool Gravity::skip(const uint16_t& step)
	{
		return step % runParams.runEachXStep != 0;
	}
	std::vector<Task> Gravity::task()
	{
		return { {this, 0, objects->size() - 1} };
	}
	void Gravity::setRunParams(RunParams concurencyParams)
	{
		runParams = concurencyParams;
	}
	RunParams Gravity::getRunParams()
	{
		return runParams;
	}
	uint8_t Gravity::getGroup()
	{
		return PRIMARY_GROUP;
	}
}
