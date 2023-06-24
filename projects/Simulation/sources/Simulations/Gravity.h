#pragma once

#include "Simulation.h"
#include "Motion.h"

#include <Eigen/Eigen>

#include <vector>


namespace projectSolar::Simulation
{
	class Gravity : public Simulation
	{
	public:
		struct Params
		{
			double gravConst = 1.0;
		};

		Gravity(Params params, RunParams concurrencyParams, Motion::MotionData* data);
		Gravity(Params params, RunParams concurrencyParams, Motion::MotionData* objects, std::vector<Motion::MotionData*> attractors);
		~Gravity() override = default;

		void prepare() override;
		void run(Task task) override;
		void swapData() override;
		void save(Serializer& serializer) override;
		void load(Serializer& serializer) override;
		bool skip(const uint16_t& step) override;
		std::vector<Task> task() override;
		RunParams getRunParams() override;
		void setRunParams(RunParams params) override;
		uint8_t getGroup() override;

		Motion::MotionData* objects;
		std::vector<Motion::MotionData*> attractors;
		int64_t skipSameIndex = -1;

		
		Params params;
		RunParams runParams;
	};
}
