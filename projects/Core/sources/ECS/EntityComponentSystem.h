#pragma once

#include "Components.h"
#include "EventHandler.h"

#include <EnTT/entt.hpp>

namespace projectSolar::ECS
{
	class EntityManager
	{
	public:
		EntityManager();
		~EntityManager() = default;
		
		entt::registry registry;

		template<typename Component, typename Storage>
		Component::Data& getData(EntityId id, Storage& storage)
		{
			return storage[registry.get<Component>(id).dataIndex];
		}

		errno_t save(std::string_view filePath) const;
		errno_t load(std::string_view filePath);
	};
}