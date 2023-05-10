#pragma once

#include "EventHandler.h"
#include "SubscriptionManager.h"
#include "Logger.h"

#include <Eigen/Eigen>
#include <atomic>


namespace projectSolar::GameLogic
{
	class PlayerManager : public EventHandler
	{
	public:
		PlayerManager();
		~PlayerManager() = default;

		void centerMapOnPlayer();

	private:
		bool m_followPlayer;
		
	public: // SLOTS
		SLOT_DECL(FOLLOW_PLAYER, bool follow);
		SLOT_DECL(CENTER_MAP_ON_PLAYER);
		SLOT_DECL(SIM_DATA_UPDATE);
	};
}
