#pragma once

#include "Simulation.h"
#include "Motion.h"

#include <Eigen/Eigen>


namespace projectSolar::Simulation
{
	class Gravity : public Simulation
	{
	public:
		struct Params
		{
			double gravConst = 1.0;
		};

		Gravity(Params params, DataStructures::DoubleBuffVector<Motion::Data>* motionData);
		~Gravity() override = default;

		void run(Task task) override;
		void swapData() override;
		void save(Serializer& serializer) override;
		void load(Serializer& serializer) override;
		bool skip(const uint16_t& step) override;
		std::vector<Task> task() override;
		RunParams runParams() override;

		DataStructures::DoubleBuffVector<Motion::Data>& data;
		Params params;

	};
}
