#pragma once

#include "EventHandler.h"

#include <vector>

namespace projectSolar
{
	class ObjectManager
	{
	public:
		enum : uint8_t
		{
			APPLICATION = 0,
			MAP_LAYER,
			GUI_LAYER,
			ENTITY_MANAGER,
			PLAYER_MANAGER,
			SIMULATION_MANAGER,
			UNIVERSE_MANAGER,
			SETTINGS_MANAGER,

			MAX_OBJECTS_NUMBER
		};

		static void init();

		template<typename ObjectType>
		static bool reg(const uint8_t& id, ObjectType* object)
		{
			if (getInstance().m_objects[id] != nullptr)
			{
				return false;
			}

			getInstance().m_objects[id] = (EventHandler*)object;
		}

		template<typename ObjectType>
		static ObjectType* get(const uint8_t& id)
		{
			return (ObjectType*)getInstance().m_objects[id];
		}

		template<typename ObjectType>
		static uint8_t find(ObjectType* input)
		{
			auto& objects = getInstance().m_objects;
			for (uint8_t i = 0; i < MAX_OBJECTS_NUMBER; i++)
			{
				if (objects[i] == (EventHandler*)input)
				{
					return i;
				}
			}

			return MAX_OBJECTS_NUMBER;
		}

	private:
		std::vector<EventHandler*> m_objects = std::vector<EventHandler*>(MAX_OBJECTS_NUMBER);

		static ObjectManager& getInstance();
		
		ObjectManager() = default;
		~ObjectManager() = default;
	};
}
