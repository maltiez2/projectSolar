#include "pch.h"

#include "Motion.h"
#include "Gravity.h"
#include "Serializer.h"
#include "Logger.h"


namespace projectSolar::Simulation
{
	Motion::Motion(Params params) :
		params(params)
	{
		LOG_ASSERT(params.precesision / params.stepSize > 0.5, "[Motion] Insufficient step precision: ", params.precesision / params.stepSize);
		unskipStep = std::max((uint16_t)(params.precesision / params.stepSize), 1ui16);
		stepSize = params.stepSize * (double)unskipStep;
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
		return false;// step% unskipStep != 0;
	}
	std::vector<Task> Motion::task()
	{
		return { {this, 0, data.size() - 1}};
	}
	RunParams Motion::runParams()
	{
		return { 1, std::max(data.size() / 1, 1ui64) };
	}


	Gravity::Gravity(Params params, DataStructures::DoubleBuffVector<Motion::Data>* motionData) :
		data(*motionData),
		params(params)
	{
	}
	void Gravity::run(Task task)
	{
		PROFILE_FUNCTION();
		
		auto& initial = data.getData();
		auto& result = data.getBuffer();

		for (size_t index = task.start; index <= task.last; index++)
		{
			result[index].force = { 0.0, 0.0, 0.0 };

			for (size_t objIndex = 0; objIndex < data.size(); objIndex++)
			{

				if (!data.isValid(objIndex))
				{
					continue;
				}
				if (objIndex == index)
				{
					continue;
				}

				Eigen::Vector3d direction = initial[objIndex].position - initial[index].position;
				double distance = direction.norm();
				result[index].force += direction * initial[objIndex].mass * initial[index].mass / distance / distance / distance;
			}

			result[index].force = result[index].force * params.gravConst;
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
		return false;//step % 2ui16 != 1ui16;
	}
	std::vector<Task> Gravity::task()
	{
		return { {this, 0, data.size() - 1} };
	}
	RunParams Gravity::runParams()
	{
		return { 3, std::max(data.size() / 100, 1ui64) };
	}
}
