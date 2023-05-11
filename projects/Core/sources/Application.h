#pragma once

#include "Simulation.h"
#include "Layers/Layers.h"
#include "Graphics.h"

#include "EventHandler.h"
#include "ECS/EntityComponentSystem.h"

#include <memory>

namespace projectSolar
{
	class Application : public EventHandler
	{
	public:
		std::shared_ptr<Simulation::SimulationRunner> m_simulation;
		std::shared_ptr<ECS::EntityManager> m_enitites;
		std::shared_ptr<Graphics::Window> m_window;
		
		Application(std::shared_ptr<Simulation::SimulationRunner> simulation, std::shared_ptr<ECS::EntityManager> entities);
		~Application() = default;

		void run();

	private:
		Layers::LayersManager m_layers;

		bool m_running = true;

		void processInputEvents();

	public: // SLOTS

		SLOT_DECL(CLOSE_WINDOW);
		SLOT_DECL(DEBUG_MESSAGE, std::string message);
	};
}