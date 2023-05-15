#pragma once

#include "Simulation.h"
#include "Layers/Layers.h"
#include "Graphics.h"

#include "EventHandler.h"
#include "ECS/EntityComponentSystem.h"

#include <memory>

namespace projectSolar
{
	class ApplicationEventHandler : public Events::EventHandler
	{
	public:
		ApplicationEventHandler(Application& app);
		~ApplicationEventHandler() override;

		EVENT_DECL(CLOSE_WINDOW, 10);
		EVENT_DECL(SET_RUN_SIMULATION, 11, bool run);

	private:
		Application& m_app;

		void processEvent(uint8_t eventType, uint8_t* data) override;
	};
	
	class Application
	{
	public:
		Application();
		~Application() = default;

		enum DefaultLayers : size_t
		{
			SIM_LAYER_ID = 1,
			MAP_LAYER_ID,
			GUI_LAYER_ID
		};

		void run();

		Simulation::DoubleBuffVector<Simulation::Motion::Data>& DEBUG_getSimData();

	private:
		std::shared_ptr<ApplicationEventHandler> m_eventHandler;
		std::shared_ptr<ECS::EntityComponentSystem> m_enitites;
		std::shared_ptr<Graphics::Window> m_window;
		Layers::LayersManager m_layers;
		
		std::shared_ptr<Layers::SimLayer> m_simLayer;
		std::shared_ptr<Layers::MapLayer> m_mapLayer;
		std::shared_ptr<Layers::GuiLayer> m_guiLayer;

		bool m_running = true;
		bool m_simAttached = false;

		uint32_t m_targetFPS = 30;
		float m_simLoad = 0.5;

		void processInputEvents();
		void processAppCondition();

		friend ApplicationEventHandler;
	};
}