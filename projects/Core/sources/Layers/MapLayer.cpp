#include "pch.h"

#include "MapLayer.h"
#include "GameLogic/SimulationManager.h"

namespace projectSolar::Layers
{
    MapLayer::MapLayer(std::shared_ptr<Graphics::Renderer> centralRenderer) :
        m_centralRenderer(centralRenderer)
    {
    }
    void MapLayer::process() // @TODO redo all these buffers stuff
    {
        updateData();
        updateMVP();
    
        m_vertexBuffer = std::make_shared<Graphics::VertexBuffer>(std::to_address(m_buffer.begin()), m_buffer.size() * sizeof(struct Point));
        m_layout = std::make_shared<Graphics::VertexBufferLayout>();
        m_layout->push<float>(3);
        m_layout->push<uint32_t>(1);
        m_vertexArray = std::make_shared<Graphics::VertexArray>();
        m_vertexArray->addBufer(*m_vertexBuffer, *m_layout);
        m_indexBuffer = std::make_shared<Graphics::IndexBuffer>(std::to_address(m_indices.begin()), (uint32_t)m_indices.size());
        m_shader = std::make_shared<Graphics::Shader>(c_shaderFile);
        m_shader->bind();
        glm::mat4 MVP = m_proj * m_view * m_model;
        m_shader->setUniformMat4f("u_MVP", MVP);

        m_centralRenderer->draw(*m_vertexArray, *m_indexBuffer, *m_shader, (uint32_t)m_indices.size());

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
    void MapLayer::setCameraOn(size_t motionDataIndex)
    {
        auto& data = Com::get().simulation->getMotionData();
        
        LOG_ASSERT(data.size() > motionDataIndex, "[MapLayer] Motion data size is less then ", motionDataIndex);
        auto& position = data[motionDataIndex];
        setCameraPosition((float)position.position[0], (float)position.position[1], (float)position.position[2]);
    }

    void MapLayer::updateData()
    {
        auto& data = Com::get().simulation->getMotionData();

        m_buffer.resize(data.size());
        m_indices.resize(data.size());

        LOG_ASSERT((m_indices.size() < (1ui64 << 16)), "[MapLayer] updateData - m_indices size is greater then max uint16_t")
        for (size_t i = 0; i < m_indices.size(); i++)
        {
            m_indices[i] = (uint16_t)i;
        }

        for (size_t i = 0; i <= 0; i++)
        {
            auto& element = data[i];
            m_buffer[i] = Point((float)element.position[0], (float)element.position[1], (float)element.position[2], 1);
        }

        for (size_t i = 1; i <= 1; i++)
        {
            auto& element = data[i];
            m_buffer[i] = Point((float)element.position[0], (float)element.position[1], (float)element.position[2], 2);
        }

        for (size_t i = 2; i < data.size(); i++)
        {
            auto& element = data[i];
            m_buffer[i] = Point((float)element.position[0], (float)element.position[1], (float)element.position[2], 0);
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
