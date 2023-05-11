#include "ObjectManager.h"

namespace projectSolar
{
	ObjectManager& ObjectManager::get()
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
		initialized = true;
		return;
	}
}
