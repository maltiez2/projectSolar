#pragma once

#include "ECS/EntityComponentSystem.h"
#include "EventHandler.h"
#include "SubscriptionManager.h"
#include "Logger.h"


namespace projectSolar::GameLogic
{
	class SettingsManager : public EventHandler
	{
	public:
		SettingsManager(ECS::EntityManager& entityManager, const Components::VideoSettings& videoParams, const Components::GameSettings& gameParams, const size_t& threadsNumber = 1);
		~SettingsManager() = default;

		entt::entity getVideoSettings();
		entt::entity getGameSettings();

	private:
		ECS::EntityManager& m_entityManager;
		entt::entity m_currentVideoSettings;
		entt::entity m_currentGameSettings;

	public: // SLOTS
		SLOT_DECL(RESOLUTION_CHANGED, uint32_t width; uint32_t height);
	};
}