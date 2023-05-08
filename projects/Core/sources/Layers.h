#pragma once

#include "Graphics.h"
#include "Simulation.h"
#include "Events/EventHandler.h"

#include <vector>
#include <map>


namespace projectSolar
{
	class Layer : public Events::EventHandler
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
		~LayersManager();

		void draw();
		bool onEvent(Graphics::InputEvent* ev);

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
		MapLayer(Graphics::Renderer* centralRenderer, Simulation::SimulationRunner* simulation);
		~MapLayer() override;

		void draw() override;
		void onEvent(Graphics::InputEvent* ev) override;

		void setMVP();
		void setProj(const glm::mat4& proj);
		void setModel(const glm::mat4& model);
		void setView(const glm::mat4& view);

	private:
		struct Point
		{
			float x;
			float y;
			float z;
			uint32_t type;
		};

		const char* shaderFile = "resources/shaders/map.shader";

		Graphics::Renderer& m_centralRenderer;
		Simulation::SimulationRunner& m_simulation;
		Graphics::Shader m_shader;
		Graphics::VertexBufferLayout m_layout;
		Graphics::VertexArray m_vertexArray;
		std::vector<Point> m_buffer = {};
		std::vector<uint32_t> m_indices = {};
		glm::mat4 m_proj;
		glm::mat4 m_model;
		glm::mat4 m_view;

		Graphics::VertexBuffer* m_vertexBuffer;
		Graphics::IndexBuffer* m_indexBuffer;

		void updateData();
	};
	class GuiLayer : public Layer
	{
	public:
		GuiLayer(Window* window, GuiWindowsManager* guiWindows, bool blockEvents = true);
		~GuiLayer() override = default;

		void draw() override;
		void onEvent(InputEvent* ev) override;

	private:
		GuiRenderer m_renderer;
		bool m_blockEvents;
	};
}
