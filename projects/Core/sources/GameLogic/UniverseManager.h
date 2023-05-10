#pragma once

#include "ECS/EntityComponentSystem.h"
#include "EventHandler.h"
#include "SubscriptionManager.h"
#include "Logger.h"


namespace projectSolar::GameLogic
{
	class UniverseManager : public EventHandler
	{
	public:
		UniverseManager(ECS::EntityManager& entityManager, const Components::Universe& params, const size_t& threadsNumber = 1);
		~UniverseManager() = default;

		entt::entity getUniverse();

	private:
		ECS::EntityManager& m_entityManager;
		entt::entity m_currentUniverse;

	public: // SLOTS

	};
}
