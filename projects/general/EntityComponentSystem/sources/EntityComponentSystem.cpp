#include "EntityComponentSystem.h"
#include "Serialization.h"\

namespace projectSolar::ECS
{
	// ENTITY
	EntityId::EntityId() :
		value((entt::entity)0)
	{
	}
	EntityId::EntityId(const entt::entity& input) :
		value(input)
	{
	}
	EntityId::EntityId(const uint64_t& input) :
		value(entt::entity(input))
	{
	}
	EntityId::operator entt::entity() const
	{
		return value;
	}
	EntityId::operator uint64_t() const
	{
		return (uint64_t)value;
	}

	// ENTITY MANAGER
	errno_t EntityManager::save(std::string_view filePath) const
	{
		return ECS::Serializer::serialize<ECS_ALL_COMPONENTS>(entities, filePath);
	}
	errno_t EntityManager::load(std::string_view filePath)
	{
		entities.clear();
		return ECS::Serializer::deserialize<ECS_ALL_COMPONENTS>(entities, filePath);
	}
}
