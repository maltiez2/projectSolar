#pragma once

#include <Eigen/Core>
#include <EnTT/entt.hpp>
#include <uuid_v4/uuid_v4.h>
#include <shared_mutex>
#include <memory>

#define PPCAT_NX(A, B) A ## B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define COMPONENT_DECL(name, ...) struct name {enum : uint16_t { TYPE = __LINE__ }; static std::shared_ptr<std::shared_mutex> mutex; __VA_ARGS__}
#define COMPONENT_INIT(name) Components::name::mutex = std::shared_ptr<std::shared_mutex>(new std::shared_mutex())


#define ECS_ALL_COMPONENTS	Components::Game,\
							Components::Object,\
							Components::GameSettings,\
							Components::VideoSettings,\
							Components::SimulationSettings,\
							Components::Attractor,\
							Components::Attractant,\
							Components::Propulsed

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
		
		// General components
		COMPONENT_DECL(Game);

		COMPONENT_DECL(Object,
			UUIDv4::UUID id;
		);

		COMPONENT_DECL(GameSettings,
			float simulationLoadFactor;
			uint16_t simulationInitialStepsNumber;
			float simulationStepsAdjustBias;
		);

		COMPONENT_DECL(VideoSettings,
			uint32_t width;
			uint32_t height;
			bool fullScreen;
			uint8_t targetFPS;
		);

		COMPONENT_DECL(SimulationSettings,
			double gravitationalConstant;
			double timeStep;
		);

		// Gravitational objects
		COMPONENT_DECL(Attractor,
			size_t dataIndex;
		);
		COMPONENT_DECL(Attractant,
			size_t dataIndex;
		);
		COMPONENT_DECL(Propulsed,
			size_t dataIndex;
		);

		// Initializes static variables inside components
		void init()
		{
			COMPONENT_INIT(Game);
			COMPONENT_INIT(Object);
			COMPONENT_INIT(GameSettings);
			COMPONENT_INIT(VideoSettings);
			COMPONENT_INIT(SimulationSettings);

			COMPONENT_INIT(Attractor);
			COMPONENT_INIT(Attractant);
			COMPONENT_INIT(Propulsed);
		}
	}
}