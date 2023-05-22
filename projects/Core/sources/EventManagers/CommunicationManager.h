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

	namespace EventManagers
	{
		class SimulationManager;
		class GuiManager;
		class MapManager;
		class SavesManager;
	};

	namespace ECS
	{
		class EntityComponentSystem;
	}

	class CommunicationManager : public Events::SubscriptionManager
	{
	public:
		EVENT_DECL(SIMULATION_UPDATED, 1, float secondsPerStep; size_t stepsPerFrame; float timePerSecond);
		EVENT_DECL(GUI_DRAWN, 2);
		EVENT_DECL(MAP_DRAWN, 3);
		EVENT_DECL(GUI_UPDATED, 4);
		EVENT_DECL(COMPONENT_CREATED, 5, uint16_t componentType; uint32_t entity);
		EVENT_DECL(COMPONENT_UPDATED, 6, uint16_t componentType; uint32_t entity);
		EVENT_DECL(COMPONENT_REMOVED, 7, uint16_t componentType; uint32_t entity);
		
		std::shared_ptr<ApplicationEventHandler> Application;
		std::shared_ptr<ECS::EntityComponentSystem> ECS;
		std::shared_ptr<EventManagers::SimulationManager> simulation;
		std::shared_ptr<EventManagers::GuiManager> GUI;
		std::shared_ptr<EventManagers::MapManager> Map;
		std::shared_ptr<EventManagers::SavesManager> Saves;
		
		static void init(const size_t& threadsNumber = 1);
		static CommunicationManager& get(const size_t& threadsNumber = 1);

		template<typename SubscriberType>
		static void subsribeToEvent(uint8_t eventId, const std::shared_ptr<SubscriberType>& subscriber)
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
