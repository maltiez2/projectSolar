#pragma once

#include "Components.h"
#include "EventHandler.h"
#include "GameLogic/CommunicationManager.h"
#include "GameLogic/SimulationManager.h"
#include "Logger.h"

#include <EnTT/entt.hpp>
#include <uuid_v4/endianness.h>
#include <uuid_v4/uuid_v4.h>


namespace projectSolar::ECS
{
	class EntityComponentSystem
	{
	public:
		EntityComponentSystem();
		~EntityComponentSystem() = default;

		template<typename Component, typename Storage>
		Component::Data& getData(EntityId id, Storage& storage)
		{
			return storage[m_registry.get<Component>(id).dataIndex];
		}

		entt::entity create()
		{
			UUIDv4::UUID uuid = m_uuidGenerator.getUUID();
			entt::entity entity = m_registry.create();
			
			std::unique_lock lock(*Components::Object::mutex);
			m_registry.emplace<Components::Object>(entity, uuid);
			lock.unlock();
			
			EMIT_EVENT(COMPONENT_CREATED, Components::Object::TYPE, (uint32_t)entity);
			return entity;
		}
		void destroy(entt::entity entity)
		{
			EMIT_EVENT(COMPONENT_REMOVED, Components::Object::TYPE, (uint32_t)entity);
			m_registry.destroy(entity);
		}
		bool exists(entt::entity entity)
		{
			return m_registry.valid(entity);
		}
		uint16_t version(entt::entity entity)
		{
			return m_registry.current(entity);
		}

		template<typename Component, typename ... Args>
		void insert(entt::entity entity, const Args& ... args)
		{
			LOG_ASSERT(exists(entity), "[EntityManager] Entity '", (uint32_t)entity, "' does not exists");
			LOG_ASSERT(!has<Component>(entity), "[EntityManager] Entity '", (uint32_t)entity, "' already has component: ", Component::TYPE);
			std::unique_lock lock(*Component::mutex);
			m_registry.emplace<Component>(entity, args...);
			lock.unlock();
			EMIT_EVENT(COMPONENT_CREATED, Component::TYPE, (uint32_t)entity);
		}
		template<typename Component, typename ... Args>
		void replace(entt::entity entity, const Args& ... args)
		{
			LOG_ASSERT(exists(entity), "[EntityManager] Entity '", (uint32_t)entity, "' does not exists");
			LOG_ASSERT(has<Component>(entity), "[EntityManager] Entity '", (uint32_t)entity, "' does not have component: ", Component::TYPE);
			std::unique_lock lock(*Component::mutex);
			m_registry.replace<Component>(entity, args...);
			lock.unlock();
			EMIT_EVENT(COMPONENT_UPDATED, Component::TYPE, (uint32_t)entity);
		}
		template<typename Component, typename ... Args>
		void update(entt::entity entity, const Args& ... args)
		{
			std::unique_lock lock(*Component::mutex);
			m_registry.emplace_or_replace<Component>(entity, args...);
			lock.unlock();
			EMIT_EVENT(COMPONENT_UPDATED, Component::TYPE, (uint32_t)entity);
		}
		template<typename Component>
		void remove(entt::entity entity)
		{
			LOG_ASSERT(exists(entity), "[EntityManager] Entity '", (uint32_t)entity, "' does not exists");
			LOG_ASSERT(has<Component>(entity), "[EntityManager] Entity '", (uint32_t)entity, "' does not have component: ", Component::TYPE);
			std::unique_lock lock(*Component::mutex);
			m_registry.erase<Component>(entity);
			lock.unlock();
			EMIT_EVENT(COMPONENT_REMOVED, Component::TYPE, (uint32_t)entity);
		}
		template<typename Component>
		void removeIfHas(entt::entity entity)
		{
			std::unique_lock lock(*Component::mutex);
			size_t removed = m_registry.remove<Component>(entity);
			lock.unlock();
			if (removed > 0)
			{
				EMIT_EVENT(COMPONENT_REMOVED, Component::TYPE, (uint32_t)entity);
			}
		}
		
		template<typename Component>
		bool has(entt::entity entity)
		{
			std::shared_lock lock(*Component::mutex);
			return m_registry.all_of<Component>(entity);
		}
		template<typename ... Components>
		bool hasAll(entt::entity entity) // Thread unsafe, call under specific component locks
		{
			return m_registry.all_of<Components...>(entity);
		}
		template<typename ... Components>
		bool hasAny(entt::entity entity) // Thread unsafe, call under specific component locks
		{
			return m_registry.any_of<Components...>(entity);
		}
		template<typename Component>
		const Component& get(entt::entity entity)
		{
			LOG_ASSERT(exists(entity), "[EntityManager] Entity '", (uint32_t)entity, "' does not exists");
			LOG_ASSERT(has<Component>(entity), "[EntityManager] Entity '", (uint32_t)entity, "' does not have component: ", Component::TYPE);
			const auto& constRegistry = m_registry;
			std::shared_lock lock(*Component::mutex);
			return constRegistry.get<Component>(entity);
		}
		template<typename ... Components>
		decltype(auto) get(entt::entity entity) // Thread unsafe, call under specific component locks
		{
			LOG_ASSERT(exists(entity), "[EntityManager] Entity '", (uint32_t)entity, "' does not exists");
			return m_registry.get<Components...>(entity);
		}

		errno_t save(std::string_view filePath) const;
		errno_t load(std::string_view filePath);

		const entt::registry& getRegistry() // @TODO This should not be needed
		{
			return m_registry;
		}
		template<typename Component>
		entt::view<Component> getView()
		{
			return m_registry.view<Component>();
		}
		template<typename Component, typename ... Components>
		decltype(auto) getView()
		{
			return m_registry.view<Component, Components...>();
		}

	private:
		entt::registry m_registry;
		UUIDv4::UUIDGenerator<std::mt19937_64> m_uuidGenerator;

		friend GameLogic::SimulationManager; // @TODO This should not be needed, maybe
	};
}