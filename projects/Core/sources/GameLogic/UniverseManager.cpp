#include "UniverseManager.h"

namespace projectSolar::GameLogic
{
	UniverseManager::UniverseManager(ECS::EntityManager& entityManager, const Components::Universe& params, const size_t& threadsNumber) :
		EventHandler(threadsNumber),
		m_entityManager(entityManager)
	{
		m_currentUniverse = m_entityManager.registry.create();
		m_entityManager.registry.emplace<Components::Universe>(m_currentUniverse, params);
	}
	entt::entity UniverseManager::getUniverse()
	{
		return m_currentUniverse;
	}
}
