#pragma once

#include "EntityStack.h"

#include <vector>
#include <map>
#include <memory>
#include <shared_mutex>

namespace projectSolar::Graphics
{
	class InputEvent;
}

namespace projectSolar::Layers
{	
	class Layer
	{
	public:
		Layer() = default;
		virtual ~Layer() = default;

		virtual void process() = 0;
		virtual void onEvent(projectSolar::Graphics::InputEvent* ev) = 0;
	};

	class LayersManager : public EntityStack<Layer>
	{
	public:
		LayersManager() = default;
		~LayersManager() = default;

		void process();
		bool onEvent(Graphics::InputEvent* ev);
	};
}
