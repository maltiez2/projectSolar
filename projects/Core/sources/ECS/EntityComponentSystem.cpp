#include "EntityComponentSystem.h"
#include "Components.h"
#include "Serialization.h"


namespace projectSolar::ECS
{
	// ENTITY
	EntityId::EntityId() :
		value((entt::entity)0)
	{
		Components::init();
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
	EntityComponentSystem::EntityComponentSystem()
	{
	}
	
	errno_t EntityComponentSystem::save(std::string_view filePath) const
	{
		return ECS::Serializer::serialize<ECS_ALL_COMPONENTS>(m_registry, filePath);
	}
	errno_t EntityComponentSystem::load(std::string_view filePath)
	{
		m_registry.clear();
		return ECS::Serializer::deserialize<ECS_ALL_COMPONENTS>(m_registry, filePath);
	}
}
