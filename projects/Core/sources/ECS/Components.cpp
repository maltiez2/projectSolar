#include "Components.h"


namespace projectSolar::Components
{
	COMPONENT_IMPL(Game);
	COMPONENT_IMPL(Object);
	COMPONENT_IMPL(GameSettings);
	COMPONENT_IMPL(VideoSettings);
	COMPONENT_IMPL(SimulationSettings);

	COMPONENT_IMPL(Attractor);
	COMPONENT_IMPL(Attractant);
	COMPONENT_IMPL(Propulsed);

	void init()
	{
		COMPONENT_INIT(Game)
		COMPONENT_INIT(Object)
		COMPONENT_INIT(GameSettings)
		COMPONENT_INIT(VideoSettings)
		COMPONENT_INIT(SimulationSettings)

		COMPONENT_INIT(Attractor)
		COMPONENT_INIT(Attractant)
		COMPONENT_INIT(Propulsed)
	}
}
