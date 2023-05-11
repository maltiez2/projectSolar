#include "SettingsManager.h"

namespace projectSolar::GameLogic
{

	SettingsManager::SettingsManager(ECS::EntityManager& entityManager, const Components::VideoSettings& videoParams, const Components::GameSettings& gameParams, const size_t& threadsNumber) :
		EventHandler(threadsNumber),
		m_entityManager(entityManager)
	{
		m_currentGameSettings = m_entityManager.registry.create();
		m_currentVideoSettings = m_entityManager.registry.create();
		m_entityManager.registry.emplace<Components::GameSettings>(m_currentGameSettings, gameParams);
		m_entityManager.registry.emplace<Components::VideoSettings>(m_currentVideoSettings, videoParams);
	}

	entt::entity SettingsManager::getVideoSettings()
	{
		return m_currentVideoSettings;
	}

	entt::entity SettingsManager::getGameSettings()
	{
		return m_currentGameSettings;
	}

	SLOT_IMPL(SettingsManager, RESOLUTION_CHANGED)
	{
		uint32_t width = data->width;
		uint32_t height = data->height;
		
		m_entityManager.registry.patch<Components::VideoSettings>(m_currentVideoSettings, [width, height](Components::VideoSettings& settings)
			{ 
				settings.width = width;
				settings.height = height;
			}
		);
	}
}
