#pragma once

#include "EventHandler.h"

#include <memory>


#define EMIT_EVENT(eventName, ...) SEND_EVENT(eventName, Com, &Com::get(), __VA_ARGS__)


namespace projectSolar
{
	class CommunicationManager;
	using Com = CommunicationManager;
	
	class Application;
	class ApplicationEventHandler;

	namespace GameLogic
	{
		class SimulationManager;
		class GuiManager;
		class MapManager;
	};

	namespace ECS
	{
		class EntityManager;
	}

	class CommunicationManager : public Events::SubscriptionManager
	{
	public:
		// EVENTS
		EVENT_DECL(SIMULATION_UPDATED, 1, float secondsPerStep; size_t stepsPerFrame);
		EVENT_DECL(GUI_DRAWN, 2);
		EVENT_DECL(MAP_DRAWN, 3);
		EVENT_DECL(GUI_UPDATED, 4);
		//
		
		std::shared_ptr<ApplicationEventHandler> Application;
		std::shared_ptr<ECS::EntityManager> ECS;
		std::shared_ptr<GameLogic::SimulationManager> simulation;
		std::shared_ptr<GameLogic::GuiManager> GUI;
		std::shared_ptr<GameLogic::MapManager> Map;
		
		static void init(const size_t& threadsNumber = 1);
		static CommunicationManager& get(const size_t& threadsNumber = 1);

		template<typename SubscriberType>
		static void subsribeOnEvent(uint8_t eventId, const std::shared_ptr<SubscriberType>& subscriber)
		{
			get().subscribe(eventId, subscriber);
		}

		template<typename SubscriberType>
		static void unsubsribeFromEvent(uint8_t eventId, const std::shared_ptr<SubscriberType>& subscriber)
		{
			get().unsubscribe(eventId, subscriber);
		}

	private:
		void processEvent(uint8_t eventType, uint8_t* data) override;
		
		explicit CommunicationManager(const size_t& threadsNumber = 1);
		~CommunicationManager() override;
	};
}
