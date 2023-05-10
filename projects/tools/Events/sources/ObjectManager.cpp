#include "Objectmanager.h"

namespace projectSolar
{
	ObjectManager& ObjectManager::getInstance()
	{
		static ObjectManager instance;
		return instance;
	}

	void ObjectManager::init()
	{
		static bool initialized;
		if (initialized)
		{
			return;
		}
		ObjectManager& inst = getInstance();
		for (uint8_t i = 0; i < MAX_OBJECTS_NUMBER; i++)
		{
			inst.m_objects[i] = nullptr;
		}
		initialized = true;
		return;
	}
}
