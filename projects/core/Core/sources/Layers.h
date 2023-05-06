#pragma once

#include "Logger.h"

#include "CentralRenderer.h"
#include "GUIRenderer.h"
#include "SimulationRunner.h"

#include <vector>
#include <map>
#include <string>

namespace projectSolar
{
	class Layer
	{
	public:
		Layer() = default;
		virtual ~Layer() = default;

		virtual void draw() = 0;
	};

	class LayersManager
	{
	public:
		LayersManager() = default;
		~LayersManager();

		void draw();

		template<typename LayerType, typename ... Args>
		Layer* add(const size_t& id, bool draw, const Args& ... args)
		{
			if (m_attached.contains(id))
			{
				LOG_ERROR("[LayersManager][add()] Layer with id '" + std::to_string(id) + "' already exists");
				return nullptr;
			}
			
			auto* layer = new LayerType(args...);

			m_attached.try_emplace(id, layer);
			m_layers.emplace_back(layer);

			return layer;
		}

		template<typename LayerType>
		LayerType* get(const size_t& id)
		{
			if (!m_attached.contains(id))
			{
				LOG_ERROR("[LayersManager][get()] Layer with id '" + std::to_string(id) + "' does not exist");
				return nullptr;
			}

			return (LayerType*)m_attached.at(id);
		}

		bool attach(const std::size_t& id, Layer* layer);
		Layer* detach(const std::size_t& id);

	private:
		std::vector<Layer*> m_layers;
		std::map<size_t, Layer*> m_attached;
	};

	class MapLayer : public Layer
	{
	public:
		MapLayer(Renderer* centralRenderer, Simulation::SimulationRunner* simulation);
		~MapLayer() override;

		void draw() override;

		void setMVP(const glm::mat4& mvp);

	private:
		struct Point
		{
			float x;
			float y;
			float z;
			uint32_t type;
		};

		const char* shaderFile = "resources/shaders/map.shader";

		Renderer& m_centralRenderer;
		Simulation::SimulationRunner& m_simulation;
		Shader m_shader;
		VertexBufferLayout m_layout;
		VertexArray m_vertexArray;
		std::vector<Point> m_buffer = {};
		std::vector<uint32_t> m_indices = {};

		VertexBuffer* m_vertexBuffer;
		IndexBuffer* m_indexBuffer;

		void updateData();
	};

	class GuiLayer : public Layer
	{
	public:
		GuiLayer(Window* window, GuiWindowsManager* guiWindows);
		~GuiLayer() override = default;

		void draw() override;

	private:
		GuiRenderer m_renderer;
	};
}
