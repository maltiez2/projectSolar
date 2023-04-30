#pragma once

#include <Eigen/Core>

#include <EnTT/entt.hpp>

#define ECS_ALL_COMPONENTS Components::Universe,   Components::Object,    Components::Attractor,\
                           Components::Attractant, Components::Propulsed

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
		struct Universe
		{
			double gravitationalConstant;
		};
		
		struct Object
		{
			size_t parent;
		};

		struct Attractor
		{
			size_t dataIndex;

			struct Data
			{
				double mass;
				Eigen::Vector3d position;
				Eigen::Vector3d velocity;
			};
		};

		struct Attractant
		{
			size_t dataIndex;

			struct Data
			{
				double mass;
				Eigen::Vector3d position;
				Eigen::Vector3d velocity;
			};
		};

		struct Propulsed
		{
			size_t dataIndex;

			struct Data
			{
				double mass;
				Eigen::Vector3d position;
				Eigen::Vector3d velocity;
				Eigen::Vector3d propulsion;
			};
		};
	}
}