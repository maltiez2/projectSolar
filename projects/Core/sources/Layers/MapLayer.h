#pragma once
#pragma once

#include "Graphics.h"
#include "Simulation.h"
#include "EventHandler.h"
#include "Layers.h"

#include <vector>
#include <memory>


namespace projectSolar::Layers
{
	class MapLayer : public Layer
	{
	public:
		MapLayer(std::shared_ptr<Graphics::Renderer> centralRenderer, std::shared_ptr<Simulation::SimulationRunner> simulation);
		~MapLayer() = default;

		void draw() override;
		void onEvent(Graphics::InputEvent* ev) override;

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

		const char* c_shaderFile = "resources/shaders/map.shader";

		std::shared_ptr<Graphics::Renderer> m_centralRenderer;
		std::shared_ptr<Simulation::SimulationRunner> m_simulation;

		std::shared_ptr<Graphics::VertexBufferLayout> m_layout;
		std::shared_ptr<Graphics::Shader>		m_shader;
		std::shared_ptr<Graphics::VertexArray>	m_vertexArray;
		std::shared_ptr<Graphics::VertexBuffer>	m_vertexBuffer;
		std::shared_ptr<Graphics::IndexBuffer>	m_indexBuffer;
		
		std::vector<Point>    m_buffer = {};
		std::vector<uint32_t> m_indices = {};
		glm::mat4 m_proj;
		glm::mat4 m_model;
		glm::mat4 m_view;

		void updateData();

	public: // SLOTS
		SLOT_DECL(SET_PROJ, uint32_t width; uint32_t height; float scale);
		SLOT_DECL(SET_VIEW, float x; float y; float z);
		SLOT_DECL(SET_MODEL, float x; float y; float z);
		SLOT_DECL(TRANSLATE_VIEW, float x; float y; float z);
		SLOT_DECL(TRANSLATE_MODEL, float x; float y; float z);
	};
}
