#pragma once

#include <memory>
#include <chrono>

namespace projectSolar
{
	namespace ECS
	{
		class EntityComponentSystem;
	}
	namespace Graphics
	{
		class Window;
	}
	namespace Layers
	{
		class LayersManager;
		class MapLayer;
		class GuiLayer;
	}
	namespace Simulation
	{
		class SimulationRunner;
	}
	
	class ApplicationEventHandler;
	
	class Application
	{
	public:
		Application();
		~Application() = default;

		enum DefaultLayers : size_t
		{
			MAP_LAYER_ID,
			GUI_LAYER_ID
		};

		void run();

	private:
		std::shared_ptr<ApplicationEventHandler> m_eventHandler;
		std::shared_ptr<ECS::EntityComponentSystem> m_enitites;
		std::shared_ptr<Simulation::SimulationRunner> m_simulation;
		std::shared_ptr<Graphics::Window> m_window;
		std::shared_ptr<Layers::LayersManager> m_layers;
		std::shared_ptr<Layers::MapLayer> m_mapLayer;
		std::shared_ptr<Layers::GuiLayer> m_guiLayer;

		bool m_running = true;

		void init();
		void processInputEvents();

		friend ApplicationEventHandler;
	};
}