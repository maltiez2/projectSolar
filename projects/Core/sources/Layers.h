#pragma once

#include "Graphics.h"
#include "Simulation.h"
#include "EventHandler.h"
#include "SubscriptionManager.h"

#include <vector>
#include <map>
#include <shared_mutex>


namespace projectSolar
{
	class Layer : public EventHandler
	{
	public:
		Layer();
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
		~MapLayer();

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

	public: // SLOTS
		SLOT_DECL(SET_PROJ, uint32_t width; uint32_t height; float scale);
		SLOT_DECL(SET_VIEW, float x; float y; float z);
		SLOT_DECL(SET_MODEL, float x; float y; float z);
		SLOT_DECL(TRANSLATE_VIEW, float x; float y; float z);
		SLOT_DECL(TRANSLATE_MODEL, float x; float y; float z);
	};
	class GuiLayer : public Layer
	{
	public:
		GuiLayer(Graphics::Window* window, Graphics::GuiWindowsManager* guiWindows, bool blockEvents = true);
		~GuiLayer() = default;

		void draw() override;
		void onEvent(Graphics::InputEvent* ev) override;

	private:
		Graphics::GuiRenderer m_renderer;
		bool m_blockEvents;
	};
}
