#include "pch.h"

#include "SavesManager.h"
#include "Logger.h"

#include "CommunicationManager.h"
#include "ECS/EntityComponentSystem.h"

#include "Layers/Layer.h"


namespace projectSolar::EventManagers
{
	SavesManager::SavesManager(std::shared_ptr<Layers::LayersManager> layers) :
		m_layers(layers)
	{
	}

	SavesManager::~SavesManager()
	{
		destroyWorkers();
	}

	void SavesManager::save(std::string saveName)
	{
		LOG_DEBUG("Saving to: ", saveName);
		std::string filePath = saveFilePath(saveName);
		m_layers->save(std::format("{}.{}", filePath, "layer"));
		Com::get().ECS->save(std::format("{}.{}", filePath, "ecs"));
	}
	void SavesManager::load(std::string saveName)
	{
		LOG_DEBUG("Loading from: ", saveName);
		std::string filePath = saveFilePath(saveName);
		m_layers->load(std::format("{}.{}", filePath, "layer"));
		Com::get().ECS->load(std::format("{}.{}", filePath, "ecs"));
	}

	void SavesManager::processEvent(uint8_t eventType, uint8_t* data)
	{
		switch (eventType)
		{			EVENTS_DEF_UNKNOWN();
			EVENT_DEF(SAVE);
			{
				std::string saveName(std::begin(eventData.saveName), std::end(eventData.saveName));
				save(saveName.c_str());
			}
			EVENT_DEF(LOAD);
			{
				std::string saveName(std::begin(eventData.saveName), std::end(eventData.saveName));
				load(saveName.c_str());
			}
			EVENTS_DEF_DEFAULT();
			break;
		}
	}

	std::string SavesManager::saveFilePath(const std::string& saveName) const
	{
		std::string saveNameStripped = saveName.c_str();
		std::filesystem::path filePath = std::filesystem::current_path();
		filePath.append(savesDirectory);
		filePath.append(saveNameStripped);
		if (!std::filesystem::exists(filePath))
		{
			std::filesystem::create_directories(filePath);
		}
		filePath.append(saveNameStripped);
		return filePath.generic_string();
	}
}
