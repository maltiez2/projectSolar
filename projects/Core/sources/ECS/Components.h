#pragma once

#include <EnTT/entt.hpp>
#include <uuid_v4/uuid_v4.h>
#include <shared_mutex>
#include <array>

#define PPCAT_NX(A, B) A ## B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define COMPONENT_DECL(name, ...) struct name {enum : uint16_t { TYPE = __LINE__ }; static std::shared_mutex& mutex(); __VA_ARGS__}
#define COMPONENT_IMPL(name) std::shared_mutex& name::mutex() {static std::shared_mutex mutex; return mutex;}
#define COMPONENT_INIT(name) name::mutex()


#define ECS_ALL_COMPONENTS	Components::Game,\
							Components::Player,\
							Components::Object,\
							Components::GameSettings,\
							Components::VideoSettings,\
							Components::SimSettings,\
							Components::SimPerformance,\
							Components::MapObject,\
							Components::CelestialObject,\
							Components::Dynamic


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

	namespace Components // Do not forget to add new components to Components.cpp
	{
		using LongTitle = std::array<char, 128>;
		using ShortTitle = std::array<char, 16>;
		
		
		// General components
		COMPONENT_DECL(Game);

		COMPONENT_DECL(Player);

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

		COMPONENT_DECL(SimSettings,
			double gravitationalConstant;
			double timeStep;
		);

		COMPONENT_DECL(SimPerformance,
			size_t stepsPerFrame;
			float secondsPerStep;
		);

		// Map objects
		COMPONENT_DECL(MapObject,
			uint32_t id;
			float colorR;
			float colorG;
			float colorB;
			float colorA;
		);

		// Gravitational objects
		COMPONENT_DECL(CelestialObject,
			LongTitle name;
		);

		COMPONENT_DECL(MotionSimulation,
			size_t index;
			uint64_t version;
			uint8_t objectClass;
		);

		// Initializes static variables inside components
		void init();
	}
}