#pragma once

#include <memory>

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
		class SimLayer;
		class MapLayer;
		class GuiLayer;
	}
	
	class ApplicationEventHandler;
	
	class Application
	{
	public:
		Application();
		~Application() = default;

		enum DefaultLayers : size_t
		{
			SIM_LAYER_ID,
			MAP_LAYER_ID,
			GUI_LAYER_ID
		};

		void run();

	private:
		std::shared_ptr<ApplicationEventHandler> m_eventHandler;
		std::shared_ptr<ECS::EntityComponentSystem> m_enitites;
		std::shared_ptr<Graphics::Window> m_window;
		std::shared_ptr<Layers::LayersManager> m_layers;
		std::shared_ptr<Layers::SimLayer> m_simLayer;
		std::shared_ptr<Layers::MapLayer> m_mapLayer;
		std::shared_ptr<Layers::GuiLayer> m_guiLayer;

		bool m_running = true;
		bool m_simAttached = false;

		uint32_t m_targetFPS = 30;
		float m_simLoad = 0.5;

		void init();
		void processInputEvents();
		void processAppState();

		friend ApplicationEventHandler;
	};
}