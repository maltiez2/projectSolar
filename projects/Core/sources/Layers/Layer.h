#pragma once

#include "Graphics.h"
#include "Simulation.h"
#include "EventHandler.h"

#include <vector>
#include <map>
#include <memory>
#include <shared_mutex>


namespace projectSolar::Layers
{
	class Layer
	{
	public:
		Layer() = default;
		virtual ~Layer() = default;

		virtual void draw() = 0;
		virtual void onEvent(Graphics::InputEvent* ev) = 0;
	};

	class LayersManager
	{
	public:
		LayersManager() = default;
		~LayersManager() = default;

		void draw();
		bool onEvent(Graphics::InputEvent* ev);

		template<typename LayerType, typename ... Args>
		std::shared_ptr<LayerType> add(size_t id, bool draw, const Args& ... args)
		{
			LOG_ASSERT(!m_attached.contains(id), "[LayersManager][add()] Layer with id '" + std::to_string(id) + "' already exists")
			
			std::shared_ptr<Layer> layer = std::dynamic_pointer_cast<Layer>(std::make_shared<LayerType>(args...));

			m_attached[id] = layer;

			return std::dynamic_pointer_cast<LayerType>(layer);
		}

		template<typename LayerType>
		std::shared_ptr<LayerType> get(const size_t& id)
		{
			LOG_ASSERT(m_attached.contains(id), "[LayersManager][get()] Layer with id '" + std::to_string(id) + "' does not exist")

			return std::dynamic_pointer_cast<LayerType>(m_attached.at(id));
		}

		template<typename LayerType>
		void attach(size_t id, std::shared_ptr<LayerType> layer)
		{
			LOG_ASSERT(!m_attached.contains(id), "[LayersManager][attach()] Layer with id '", id, "' already exists")

			m_attached[id] = std::dynamic_pointer_cast<Layer>(layer);
		}

		template<typename LayerType>
		std::shared_ptr<LayerType> detach(const size_t& id)
		{
			LOG_ASSERT(m_attached.contains(id), "[LayersManager][detach()] Layer with id '", id, "' does not exist")

			std::shared_ptr<Layer> layer = m_attached.at(id);

			m_attached.erase(id);

			return std::dynamic_pointer_cast<LayerType>(layer);
		}

	private:
		std::map<size_t, std::shared_ptr<Layer>> m_attached;
	};
}
