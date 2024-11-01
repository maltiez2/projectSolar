#pragma once
#pragma once

#include "Layer.h"

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace projectSolar::Graphics
{
	class Renderer;
	class VertexBufferLayout;
	class Shader;
	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;
	class InputEvent;
}

namespace projectSolar::Layers
{
	class MapLayer : public Layer
	{
	public:
		MapLayer();
		~MapLayer() override = default;

		void process() override;
		void onEvent(projectSolar::Graphics::InputEvent* ev) override;
		void save(const std::string& filePath) override;
		void load(const std::string& filePath) override;

		void setCameraPosition(float x, float y, float z);
		void setResolution(uint32_t width, uint32_t height);
		void setCameraScale(float scale);
		void moveCameraPosition(float x, float y, float z);
		void moveCameraScale(float scaleDelta);
		void setCameraOn(size_t simulation, size_t motionDataIndex);
		void updateCamera();
		void setMouseAt(float x, float y);

		const std::vector<entt::entity>& getObjectsUnderMouse() const;


	private:
		struct Point
		{
			float x;
			float y;
			float z;
			float r;
			float g;
			float b;
			float a;
			uint32_t id;
		};

		struct Camera
		{
			glm::vec3 position = { 0.0f, 0.0f, 0.0f };
			glm::vec3 direction = { 0.0f, 0.0f, 1.0f };
			glm::vec2 resolution = { 1600.0f, 900.0f };
			float scale = 1.0f;
		};

		const char* c_shaderFile = "resources/shaders/map.shader";
		const float c_mouseDetectionRadius = 0.007f;

		std::shared_ptr<projectSolar::Graphics::Renderer> m_centralRenderer = std::make_shared<projectSolar::Graphics::Renderer>();

		std::shared_ptr<projectSolar::Graphics::VertexBufferLayout> m_layout;
		std::shared_ptr<projectSolar::Graphics::Shader>		m_shader;
		std::shared_ptr<projectSolar::Graphics::VertexArray>	m_vertexArray;
		std::shared_ptr<projectSolar::Graphics::VertexBuffer>	m_vertexBuffer;
		std::shared_ptr<projectSolar::Graphics::IndexBuffer>	m_indexBuffer;

		Camera m_currentCamera;
		bool m_cameraSetOn = false;
		size_t m_cameraObjSimulation;
		size_t m_cameraObjIndex;
		
		std::vector<Point>    m_buffer = {};
		std::vector<uint32_t> m_indices = {};
		glm::mat4 m_proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
		glm::mat4 m_model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		glm::mat4 m_view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		glm::vec2 m_mousePos = glm::vec2(0, 0);

		std::vector<entt::entity> m_objectsUnderMouse;
		std::vector<entt::entity> m_objectsDrugged;
		glm::vec3 m_mousePosInWorld = glm::vec3(0, 0, 0);

		void updateData();
		void updateMVP();
		bool checkMouse(const float& objX, const float& objY, const float& mouseX, const float& mouseY, const float& epsilonX, const float& epsilonY) const;
	};
}
