#pragma once

#include "ECS/EntityComponentSystem.h"
#include "EventHandler.h"
#include "Layers/GuiLayer.h"
#include "Logger.h"

#include <memory>


namespace projectSolar::EventManagers
{
	class GuiManager : public Events::EventHandler
	{
	public:
		GuiManager(std::shared_ptr<Layers::GuiLayer> layer, const size_t& threadsNumber = 1);
		~GuiManager() override;

		enum WidnowId : size_t
		{
			DEBUG,
			DEMO
		};

		static const size_t maxObjUnderCursor = 16;

		EVENT_DECL(SIMULATION_UPDATED, 1, float secondsPerStep; size_t stepsPerFrame);
		EVENT_DECL(GUI_UPDATED, 4);

		EVENT_DECL(MAP_OBJECTS_UNDER_CURSOR, 101, std::array<entt::entity, maxObjUnderCursor> objects);

		std::shared_ptr<Graphics::GuiWindowsManager> getWidnows();

	private:
		void setUpGUI();
		void processEvent(uint8_t eventType, uint8_t* data) override;
		void updateWidnowsInteraction();

		std::shared_ptr<Layers::GuiLayer> m_layer;
		std::shared_ptr<Graphics::GuiWindowsManager> m_windows;
	};
}