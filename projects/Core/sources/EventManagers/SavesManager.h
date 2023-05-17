#pragma once

#include "EventHandler.h"

namespace projectSolar::Layers
{
	class LayersManager;
}

namespace projectSolar::EventManagers
{
	
	
	class SavesManager : public Events::EventHandler
	{
	public:
		SavesManager(std::shared_ptr<Layers::LayersManager> layers);
		~SavesManager() override;

		void save(std::string saveName);
		void load(std::string saveName);

		EVENT_DECL(SAVE, 101, std::array<char, 128> saveName);
		EVENT_DECL(LOAD, 102, std::array<char, 128> saveName);

	private:
		const std::string savesDirectory = "saves";

		std::shared_ptr<Layers::LayersManager> m_layers;

		void processEvent(uint8_t eventType, uint8_t* data) override;

		std::string saveFilePath(const std::string& saveName) const;
	};
}