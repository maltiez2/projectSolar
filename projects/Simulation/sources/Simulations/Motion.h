#pragma once

#include "Simulation.h"
#include "DoubleBufferedContainer.h"

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
			double stepSize;
			double precesision;
		};
		
		explicit Motion(Params params);
		~Motion() override = default;

		void run(Task task) override;
		void swapData() override;
		void save(Serializer& serializer) override;
		void load(Serializer& serializer) override;
		bool skip(const uint16_t& step) override;
		std::vector<Task> task() override;
		RunParams runParams() override;

		DataStructures::DoubleBuffVector<Data> data;
		Params params;

		double stepSize;
		uint16_t unskipStep;
	};
}
