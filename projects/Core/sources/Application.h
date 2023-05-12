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
		EVENT_DECL(SET_SIMULATION_STEP, 12, double stepSize);
		EVENT_DECL(SET_SIMULATION_LOAD, 13, float framePeriodFactor;);
		EVENT_DECL(SET_SIMULATION_FPS, 14, uint8_t framesPerSecond);

	private:
		Application& m_app;

		void processEvent(uint8_t eventType, uint8_t* data) override;
	};
	
	class Application
	{
	public:
		Application(std::shared_ptr<Simulation::SimulationRunner> simulation, std::shared_ptr<ECS::EntityManager> entities);
		~Application() = default;

		void run();

	private:
		std::shared_ptr<ApplicationEventHandler> m_eventHandler;
		std::shared_ptr<Simulation::SimulationRunner> m_simulation;
		std::shared_ptr<ECS::EntityManager> m_enitites;
		std::shared_ptr<Graphics::Window> m_window;
		Layers::LayersManager m_layers;

		Simulation::SimulationRunner::Params m_simulationParams;

		bool m_running = true;
		bool m_simulating = false;

		void processInputEvents();

		friend ApplicationEventHandler;
	};
}