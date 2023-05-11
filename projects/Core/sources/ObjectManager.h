#pragma once

#include <memory>

namespace projectSolar
{
	class Application;

	namespace Layers 
	{
		class GuiLayer;
		class MapLayer;
	};

	namespace GameLogic
	{
		class PlayerManager;
		class SettingsManager;
		class SimulationManager;
		class UniverseManager;
	};

	namespace ECS
	{
		class EntityManager;
	}

	class ObjectManager
	{
	public:
		// Layers
		std::shared_ptr<Layers::GuiLayer> layerGui;
		std::shared_ptr<Layers::MapLayer> layerMap;
		// Managers
		std::shared_ptr<ECS::EntityManager> managerECS;
		std::shared_ptr<GameLogic::PlayerManager>     managerPlayer;
		std::shared_ptr<GameLogic::SettingsManager>   managerSettings;
		std::shared_ptr<GameLogic::SimulationManager> managerSimulation;
		std::shared_ptr<GameLogic::UniverseManager>   managerUniverse;
		
		static void init();
		static ObjectManager& get();

	private:
		ObjectManager() = default;
		~ObjectManager() = default;
	};
}
