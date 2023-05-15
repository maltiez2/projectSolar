#pragma once

#include "../SimulationRunner.h"
#include "../DoubleBufferedContainer.h"

#include <Eigen/Eigen>


namespace projectSolar::Simulation
{
	class Motion : public Simulation
	{
	public:
		struct Data
		{	
			double mass;
			Eigen::Vector3d position;
			Eigen::Vector3d velocity;
			Eigen::Vector3d force;
		};

		struct Params
		{
		};
		
		explicit Motion(Params params);
		~Motion() override = default;

		void run(Task task) override;
		void swapData() override;
		void save(Serializer& serializer) override;
		void load(Serializer& serializer) override;

		DoubleBuffVector<Data> data;
		Params params;

	};
}
