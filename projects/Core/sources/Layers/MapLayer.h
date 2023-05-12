#pragma once
#pragma once

#include "Graphics.h"
#include "Simulation.h"
#include "EventHandler.h"
#include "ECS/Components.h"
#include "Layer.h"
#include "GameLogic/CommunicationManager.h"

#include <vector>
#include <memory>


namespace projectSolar::Layers
{
	class MapLayer : public Layer
	{
	public:
		MapLayer(std::shared_ptr<Graphics::Renderer> centralRenderer, std::shared_ptr<Simulation::SimulationRunner> simulation);
		~MapLayer() override = default;

		void draw() override;
		void onEvent(Graphics::InputEvent* ev) override;
		
		void setCameraPosition(float x, float y, float z);
		void setResolution(uint32_t width, uint32_t height);
		void setCameraScale(float scale);

		void moveCameraPosition(float x, float y, float z);
		void moveCameraScale(float scaleDelta);

		template<typename Component>
		void setCameraOn(size_t simulationDataIndex)
		{
			LOG_ASSERT(false, "[MapLayer] Unspecified template. Index: ", simulationDataIndex)
		}
		template<>
		void setCameraOn<Components::Attractor>(size_t simulationDataIndex)
		{
			LOG_ASSERT(m_simulation->getData().attractorsData.getData().size() > simulationDataIndex, "[MapLayer] Attractors data size is less then ", simulationDataIndex);
			auto& position = m_simulation->getData().attractorsData.getData()[simulationDataIndex];
			setCameraPosition((float)position.position[0], (float)position.position[1], (float)position.position[2]);
		}
		template<>
		void setCameraOn<Components::Attractant>(size_t simulationDataIndex)
		{
			LOG_ASSERT(m_simulation->getData().attractantsData.getData().size() > simulationDataIndex, "[MapLayer] Attractants data size is less then ", simulationDataIndex);
			auto& position = m_simulation->getData().attractantsData.getData()[simulationDataIndex];
			setCameraPosition((float)position.position[0], (float)position.position[1], (float)position.position[2]);
		}
		template<>
		void setCameraOn<Components::Propulsed>(size_t simulationDataIndex)
		{
			LOG_ASSERT(m_simulation->getData().propulsedData.getData().size() > simulationDataIndex, "[MapLayer] Propulsed data size is less then ", simulationDataIndex);
			auto& position = m_simulation->getData().propulsedData.getData()[simulationDataIndex];
			setCameraPosition((float)position.position[0], (float)position.position[1], (float)position.position[2]);
		}

	private:
		struct Point
		{
			float x;
			float y;
			float z;
			uint32_t type;
		};

		struct Camera
		{
			glm::vec3 position;
			glm::vec3 direction;
			glm::vec2 resolution;
			float scale;
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

		Camera m_currentCamera;

		void updateData();
		void updateMVP();
	};
}
