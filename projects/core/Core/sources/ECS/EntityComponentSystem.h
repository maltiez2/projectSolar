#pragma once

#include "Components.h"

#include <EnTT/entt.hpp>

namespace projectSolar::ECS
{
	class EntityManager
	{
	public:
		entt::registry entities;

		template<typename Component, typename Storage>
		Component::Data& getData(EntityId id, Storage& storage)
		{
			return storage[entities.get<Component>(id).dataIndex];
		}

		errno_t save(std::string_view filePath) const;
		errno_t load(std::string_view filePath);
	};
}