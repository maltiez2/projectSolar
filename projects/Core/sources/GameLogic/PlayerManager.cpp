#include "pch.h"

#include "PlayerManager.h"

namespace projectSolar::GameLogic
{
	PlayerManager::PlayerManager() :
		EventHandler(1)
	{
		SUBSCRIBE(PlayerManager, this, SIM_DATA_UPDATE, SIM_DATA_UPDATE);
	}

	void PlayerManager::centerMapOnPlayer()
	{
		LOG_DEBUG("[PlayerManager] centerMapOnPlayer");
	}

	SLOT_IMPL(PlayerManager, FOLLOW_PLAYER)
	{
		m_followPlayer = data->follow;
	}
	SLOT_IMPL(PlayerManager, CENTER_MAP_ON_PLAYER)
	{
		centerMapOnPlayer();
	}
	SLOT_IMPL(PlayerManager, SIM_DATA_UPDATE)
	{
		if (m_followPlayer)
		{
			centerMapOnPlayer();
		}
	}
}
