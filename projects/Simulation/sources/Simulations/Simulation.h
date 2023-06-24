#pragma once

#include <vector>

namespace projectSolar::Simulation
{
	class Serializer;
	class Simulation;

	struct Task
	{
		Simulation* simulation;
		
		size_t start;
		size_t last;

		size_t size() const;
	};

	struct RunParams
	{
		size_t granularity = 1;
		size_t minimumSize = 1;
		uint16_t runEachXStep = 1;
	};

	class Simulation
	{
	public:
		Simulation() = default;
		virtual ~Simulation() = default;

		enum : uint8_t
		{
			PRIMARY_GROUP,
			SECONDARY_GROUP
		};

		virtual void prepare() = 0;
		virtual void run(Task task) = 0;
		virtual void swapData() = 0;
		virtual void save(Serializer& serializer) = 0;
		virtual void load(Serializer& serializer) = 0;
		virtual bool skip(const uint16_t& step) = 0;
		virtual std::vector<Task> task() = 0;
		virtual RunParams getRunParams() = 0;
		virtual void setRunParams(RunParams params) = 0;
		virtual uint8_t getGroup() = 0;
	};
}
