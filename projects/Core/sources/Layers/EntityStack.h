#pragma once

#include "Logger.h"

#include <map>
#include <memory>

namespace projectSolar::Layers
{
	template<typename BaseEntityType>
	class EntityStack
	{
	public:
		template<typename EntityType, typename ... Args>
		std::shared_ptr<EntityType> add(size_t id, const Args& ... args)
		{
			LOG_ASSERT(!m_attached.contains(id), "[EntityStack][add()] Entity with id '" + std::to_string(id) + "' already exists");
			LOG_ASSERT(!m_registered.contains(id), "[EntityStack][add()] Entity with id '" + std::to_string(id) + "' already registered");

			std::shared_ptr<BaseEntityType> entity = std::dynamic_pointer_cast<BaseEntityType>(std::make_shared<EntityType>(args...));
			m_attached[id] = entity;
			m_registered[id] = entity;
			return std::dynamic_pointer_cast<EntityType>(entity);
		}

		template<typename EntityType>
		std::shared_ptr<EntityType> get(const size_t& id)
		{
			LOG_ASSERT(m_attached.contains(id), "[EntityStack][get()] Entity with id '" + std::to_string(id) + "' does not exist");

			return std::dynamic_pointer_cast<EntityType>(m_attached.at(id));
		}

		template<typename EntityType>
		void attach(size_t id, std::shared_ptr<EntityType> entity)
		{
			LOG_ASSERT(!m_attached.contains(id), "[EntityStack][attach()] Entity with id '", id, "' already exists");
			LOG_ASSERT(!m_registered.contains(id), "[EntityStack][attach()] Entity with id '" + std::to_string(id) + "' already registered");

			m_attached[id] = std::dynamic_pointer_cast<BaseEntityType>(entity);
			m_registered[id] = m_attached[id];
		}

		template<typename EntityType>
		void reattach(size_t id)
		{
			LOG_ASSERT(!m_attached.contains(id), "[EntityStack][reattach()] Entity with id '", id, "' already attached");
			LOG_ASSERT(m_registered.contains(id), "[EntityStack][reattach()] Entity with id '" + std::to_string(id) + "' not registered");

			m_attached[id] = m_registered[id];
		}

		template<typename EntityType>
		std::shared_ptr<EntityType> detach(const size_t& id)
		{
			LOG_ASSERT(m_attached.contains(id), "[EntityStack][detach()] Entity with id '", id, "' does not exist");

			std::shared_ptr<BaseEntityType> entity = m_attached.at(id);
			m_attached.erase(id);
			return std::dynamic_pointer_cast<EntityType>(entity);
		}

		bool ifAttached(const size_t& id)
		{
			return m_attached.contains(id);
		}

		bool ifRegistered(const size_t& id)
		{
			return m_registered.contains(id);
		}

	protected:
		std::map<size_t, std::shared_ptr<BaseEntityType>> m_attached;
		std::map<size_t, std::shared_ptr<BaseEntityType>> m_registered;
	};
}
