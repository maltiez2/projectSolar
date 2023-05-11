#pragma once

#include <Eigen/Core>

#include <EnTT/entt.hpp>

#define ECS_ALL_COMPONENTS	Components::Universe,	Components::Object,		Components::Attractor,\
							Components::Attractant,	Components::Propulsed

namespace projectSolar
{
	namespace ECS
	{
		struct EntityId
		{
			entt::entity value;

			~EntityId() = default;

			EntityId();
			EntityId(const entt::entity& input);
			EntityId(const uint64_t& input);
			EntityId(const EntityId& input) = default;

			operator entt::entity() const;
			operator uint64_t() const;
		};
	}

	namespace Components
	{
		struct GameSettings
		{
			float simulationLoadFactor;
			uint16_t simulationInitialStepsNumber;
			float simulationStepsAdjustBias;
		};

		struct VideoSettings
		{
			uint32_t width;
			uint32_t height;
			bool fullScreen;
			uint8_t targetFPS;
		};

		struct SimulationPerformance
		{
			float secondsPerStep;
			uint16_t subStepsNumber;
		};
		
		struct Universe
		{
			double gravitationalConstant;
			double timeStep;
		};
		
		struct Object
		{
			size_t parent;
		};

		struct Attractor
		{
			size_t dataIndex;
		};

		struct Attractant
		{
			size_t dataIndex;
		};

		struct Propulsed
		{
			size_t dataIndex;
		};
	}
}