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
		virtual void save(const std::string& filePath) = 0;
		virtual void load(const std::string& filePath) = 0;
	};

	class LayersManager : public DataStructures::EntityStack<Layer>
	{
	public:
		LayersManager() = default;
		~LayersManager() = default;

		void process();
		bool onEvent(Graphics::InputEvent* ev);
		void save(const std::string& filePath) const;
		void load(const std::string& filePath) const;
	};
}
