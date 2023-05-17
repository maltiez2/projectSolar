#include "pch.h"

#include "Components.h"


namespace projectSolar::Components
{
	COMPONENT_IMPL(Game);
	COMPONENT_IMPL(Object);
	COMPONENT_IMPL(GameSettings);
	COMPONENT_IMPL(VideoSettings);
	COMPONENT_IMPL(SimSettings);
	COMPONENT_IMPL(SimPerformance);

	COMPONENT_IMPL(MapObject);

	COMPONENT_IMPL(CelestialObject);
	COMPONENT_IMPL(Dynamic);

	void init()
	{
		COMPONENT_INIT(Game);
		COMPONENT_INIT(Object);
		COMPONENT_INIT(GameSettings);
		COMPONENT_INIT(VideoSettings);
		COMPONENT_INIT(SimSettings);
		COMPONENT_INIT(SimPerformance);

		COMPONENT_INIT(MapObject);

		COMPONENT_INIT(CelestialObject);
		COMPONENT_INIT(Dynamic);
	}
}
