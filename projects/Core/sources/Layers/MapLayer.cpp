#include "pch.h"

#include "MapLayer.h"

namespace projectSolar::Layers
{
    MapLayer::MapLayer(std::shared_ptr<Graphics::Renderer> centralRenderer, std::shared_ptr<Simulation::SimulationRunner> simulation) :
        m_centralRenderer(centralRenderer),
        m_simulation(simulation)
    {
        m_proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
        m_view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        m_model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

        updateData();

        m_shader = std::make_shared<Graphics::Shader>(c_shaderFile);
        m_vertexBuffer = std::make_shared<Graphics::VertexBuffer>(std::to_address(m_buffer.begin()), m_buffer.size() * sizeof(struct Point));
        m_layout = std::make_shared<Graphics::VertexBufferLayout>();
        m_layout->push<float>(3);
        m_layout->push<uint32_t>(1);
        m_vertexArray = std::make_shared<Graphics::VertexArray>();
        m_vertexArray->addBufer(*m_vertexBuffer, *m_layout);
        m_indexBuffer = std::make_shared<Graphics::IndexBuffer>(std::to_address(m_indices.begin()), (uint32_t)m_indices.size());
    }
    void MapLayer::draw()
    {
        updateData();
        updateMVP();

        m_vertexBuffer->updateData(std::to_address(m_buffer.begin()), m_buffer.size() * sizeof(struct Point));
        m_shader->bind();
        glm::mat4 MVP = m_proj * m_view * m_model;
        m_shader->setUniformMat4f("u_MVP", MVP);

        m_centralRenderer->draw(*m_vertexArray, *m_indexBuffer, *m_shader);

        m_vertexBuffer->unbind();
        m_indexBuffer->unbind();
        m_vertexArray->unbind();
        m_shader->unbind();

        EMIT_EVENT(MAP_DRAWN);
    }
    void MapLayer::onEvent(Graphics::InputEvent* ev)
    {
        switch (ev->getEventType())
        {
        case Graphics::InputEventType::WindowResize:
        {
            auto eventData = (Graphics::WindowResizeEvent*)ev;
            setResolution(eventData->getWidth(), eventData->getHeight());
            break;
        }
        default:
            break;
        }
    }
    void MapLayer::setCameraPosition(float x, float y, float z)
    {
        m_currentCamera.position.x = x;
        m_currentCamera.position.y = y;
        m_currentCamera.position.z = z;
    }
    void MapLayer::setResolution(uint32_t width, uint32_t height)
    {
        m_currentCamera.resolution.x = (float)width;
        m_currentCamera.resolution.y = (float)height;
        LOG_DEBUG("Resolution set at:", width, ", ", height);
    }
    void MapLayer::setCameraScale(float scale)
    {
        m_currentCamera.scale = scale;
    }
    void MapLayer::moveCameraPosition(float x, float y, float z)
    {
        m_currentCamera.position.x += x;
        m_currentCamera.position.y += y;
        m_currentCamera.position.z += z;
    }
    void MapLayer::moveCameraScale(float scaleDelta)
    {
        m_currentCamera.scale += scaleDelta;
    }

    void MapLayer::updateData()
    {
        auto& dataManager = m_simulation->getData();

        size_t attractantsIndex = dataManager.attractorsData.size();
        size_t propulsesIndex = dataManager.attractantsData.size() + attractantsIndex;
        size_t bufferSize = dataManager.propulsedData.size() + propulsesIndex;

        m_buffer.resize(bufferSize);
        m_indices.resize(bufferSize);

        LOG_ASSERT((m_indices.size() < (1ui64 << 16)), "[MapLayer] updateData - m_indices size is greater then max uint16_t")
        for (size_t i = 0; i < m_indices.size(); i++)
        {
            m_indices[i] = (uint16_t)i;
        }

        auto& atorData = dataManager.attractorsData.getData();
        for (size_t i = 0; i < atorData.size(); i++)
        {
            auto& element = atorData[i];
            m_buffer[i] = Point((float)element.position[0], (float)element.position[1], (float)element.position[2], 0);
        }

        auto& atantData = dataManager.attractantsData.getData();
        for (size_t i = 0; i < atantData.size(); i++)
        {
            auto& element = atantData[i];
            m_buffer[attractantsIndex + i] = Point((float)element.position[0], (float)element.position[1], (float)element.position[2], 1);

        }

        auto& propData = dataManager.propulsedData.getData();
        for (size_t i = 0; i < propData.size(); i++)
        {
            auto& element = propData[i];
            m_buffer[propulsesIndex + i] = Point((float)element.position[0], (float)element.position[1], (float)element.position[2], 2);
        }
    }
    void MapLayer::updateMVP()
    {
        m_proj = glm::ortho(
            -0.5f * m_currentCamera.scale * m_currentCamera.resolution.x, 0.5f * m_currentCamera.scale * m_currentCamera.resolution.x,
            -0.5f * m_currentCamera.scale * m_currentCamera.resolution.y, 0.5f * m_currentCamera.scale * m_currentCamera.resolution.y,
            -1.0f * m_currentCamera.scale, 1.0f * m_currentCamera.scale
        );

        m_view = glm::translate(glm::mat4(1.0f), -1.0f * m_currentCamera.position);
    }
}
