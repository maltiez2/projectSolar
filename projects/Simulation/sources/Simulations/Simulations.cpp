#include "pch.h"

#include "Motion.h"
#include "Gravity.h"
#include "Serializer.h"


namespace projectSolar::Simulation
{
	Motion::Motion(Params params) :
		params(params)
	{
	}
	void Motion::run(Task task)
	{
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


	Gravity::Gravity(Params params, DoubleBuffVector<Motion::Data>* motionData) :
		data(*motionData),
		params(params)
	{
	}
	void Gravity::run(Task task)
	{
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
	}
	void Gravity::save(Serializer& serializer)
	{
		// Nothing to serialize
	}
	void Gravity::load(Serializer& serializer)
	{
		// Nothing to deserialize
	}
}
