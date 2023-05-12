#include "CommunicationManager.h"
#include "Logger.h"

namespace projectSolar
{
	void CommunicationManager::processEvent(uint8_t eventType, uint8_t* data)
	{
		switch (eventType)
		{
			SUBSCRIPTION_DEF(SIMULATION_UPDATED);
			SUBSCRIPTION_DEF(GUI_DRAWN);
			SUBSCRIPTION_DEF(MAP_DRAWN);
			SUBSCRIPTION_DEF(GUI_UPDATED);
			SUBSCRIPTION_DEF_DEFAULT;
		}
	}
	
	CommunicationManager::CommunicationManager(const size_t& threadsNumber) :
		Events::SubscriptionManager(threadsNumber)
	{

	}
	CommunicationManager::~CommunicationManager()
	{
	}
	
	CommunicationManager& CommunicationManager::get(const size_t& threadsNumber)
	{
		static CommunicationManager instance(threadsNumber);
		return instance;
	}

	void CommunicationManager::init(const size_t& threadsNumber)
	{
		static bool initialized;
		if (initialized)
		{
			return;
		}
		get(threadsNumber);
		initialized = true;
		return;
	}
}
