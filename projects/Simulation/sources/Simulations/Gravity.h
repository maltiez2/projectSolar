#pragma once

#include "../SimulationRunner.h"
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

		Gravity(Params params, DoubleBuffVector<Motion::Data>* motionData);
		~Gravity() override = default;

		void run(Task task) override;
		void swapData() override;
		void save(Serializer& serializer) override;
		void load(Serializer& serializer) override;

		DoubleBuffVector<Motion::Data>& data;
		Params params;

	};
}
