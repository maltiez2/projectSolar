#include "pch.h"

#include "MapLayer.h"

#include "Graphics.h"
#include "Buffers.h"
#include "InputEvents.h"

#include "EventManagers/CommunicationManager.h"
#include "EventManagers/SimulationManager.h"
#include "EventManagers/GuiManager.h"
#include "ECS/EntityComponentSystem.h"
#include "ECS/Components.h"

#include <utility>


namespace projectSolar::Layers
{
    MapLayer::MapLayer()
    {
    }
    void MapLayer::process() // @TODO redo all these buffers stuff
    {
        updateMVP();
        glm::mat4 MVP = m_proj * m_view * m_model;
        
        updateData();

        m_vertexBuffer = std::make_shared<projectSolar::Graphics::VertexBuffer>(std::to_address(m_buffer.begin()), m_buffer.size() * sizeof(struct Point));
        m_layout = std::make_shared<projectSolar::Graphics::VertexBufferLayout>();
        m_layout->push<float>(3);
        m_layout->push<float>(4);
        m_layout->push<uint32_t>(1);
        m_vertexArray = std::make_shared<projectSolar::Graphics::VertexArray>();
        m_vertexArray->addBufer(*m_vertexBuffer, *m_layout);
        m_indexBuffer = std::make_shared<projectSolar::Graphics::IndexBuffer>(std::to_address(m_indices.begin()), (uint32_t)m_indices.size());
        m_shader = std::make_shared<projectSolar::Graphics::Shader>(c_shaderFile);
        m_shader->bind();
        m_shader->setUniformMat4f("u_MVP", MVP);
        m_shader->setUniform2f("u_mouseCoords", m_mousePos.x, m_mousePos.y);
        m_shader->setUniform2f("u_mouseEpsilon", 10.0f / m_currentCamera.resolution.x, 10.0f / m_currentCamera.resolution.y);
        //LOG_DEBUG("Mouse at: ", m_mousePos.x, " : ", m_mousePos.y);

        m_centralRenderer->draw(*m_vertexArray, *m_indexBuffer, *m_shader, (uint32_t)m_indices.size());

        m_vertexBuffer->unbind();
        m_indexBuffer->unbind();
        m_vertexArray->unbind();
        m_shader->unbind();

        EMIT_EVENT(MAP_DRAWN);
    }
    void MapLayer::onEvent(projectSolar::Graphics::InputEvent* ev)
    {
        switch (ev->getEventType())
        {
        case projectSolar::Graphics::InputEventType::WindowResize:
        {
            auto eventData = (projectSolar::Graphics::WindowResizeEvent*)ev;
            setResolution(eventData->getWidth(), eventData->getHeight());
            break;
        }
        case projectSolar::Graphics::InputEventType::MouseMoved:
        {
            auto eventData = (projectSolar::Graphics::MouseMovedEvent*)ev;
            setMouseAt(eventData->getX(), eventData->getY());
            break;
        }
        default:
            break;
        }
    }
    void MapLayer::save(const std::string& filePath)
    {
        // Nothing to save yet
    }
    void MapLayer::load(const std::string& filePath)
    {
        // Nothing to load yet
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
    void MapLayer::setMouseAt(float x, float y)
    {
        m_mousePos.x = 2.0f * x / m_currentCamera.resolution.x - 1.0f;
        m_mousePos.y = 1.0f - 2.0f * y / m_currentCamera.resolution.y;
    }

    void MapLayer::updateData()
    {
        float epsilon = 0.007f;
        glm::mat4 inversView = glm::inverse(m_view);
        glm::vec4 mousePos = inversView * glm::vec4(m_mousePos, 0.0f, 0.0f);
        glm::vec4 mouseEpsilon = inversView * glm::vec4(epsilon, epsilon, 0.0f, 0.0f);

        auto& simData = Com::get().simulation->getMotionData();

        auto entities = Com::get().ECS->getView<Components::Dynamic, Components::MapObject>();

        bool isOUMEmpty = m_objectsUnderMouse.empty();

        m_buffer.clear();
        m_indices.clear();
        m_objectsUnderMouse.clear();

        for (auto entity : entities)
        {
            std::shared_lock dynamicLock(Components::Dynamic::mutex());
            const Components::Dynamic& dynamic = entities.get<Components::Dynamic>(entity);
            Simulation::Motion::Data& motion = simData[dynamic.motionDataIndex];
            dynamicLock.unlock();
            
            std::shared_lock mapObjLock(Components::MapObject::mutex());
            Components::MapObject& mapObj = entities.get<Components::MapObject>(entity);

            if (checkMouse((float)motion.position[0], (float)motion.position[1], mousePos.x, mousePos.y, mouseEpsilon.x, mouseEpsilon.y))
            {
                m_objectsUnderMouse.push_back(entity);
            }

            m_buffer.emplace_back(
                (float)motion.position[0], (float)motion.position[1], (float)motion.position[2],
                mapObj.colorR, mapObj.colorG, mapObj.colorB, mapObj.colorA,
                mapObj.id
            );
        }

        

        if (!isOUMEmpty || !m_objectsUnderMouse.empty())
        {
            std::array<entt::entity, EventManagers::GuiManager::maxObjUnderCursor> oum;
            
            for (size_t i = 0; i < oum.size(); i++)
            {
                oum[i] = entt::null;
            }
            for (size_t i = 0; i < std::min(oum.size(), m_objectsUnderMouse.size()); i++)
            {
                oum[i] = m_objectsUnderMouse[i];
            }
            
            SEND_EVENT(MAP_OBJECTS_UNDER_CURSOR, EventManagers::GuiManager, Com::get().GUI, oum);
        }

        LOG_ASSERT((m_buffer.size() < (1ui64 << 16)), "[MapLayer] updateData - m_indices size is greater then max uint16_t");
        m_indices.resize(m_buffer.size());
        for (size_t i = 0; i < m_buffer.size(); i++)
        {
            m_indices[i] = (uint16_t)i;
        }
    }
    void MapLayer::updateMVP()
    {
        m_proj = glm::ortho(
            -1.0f, 1.0f,
            -1.0f, 1.0f,
            -1.0f, 1.0f
        );

        m_view = glm::scale(glm::translate(glm::mat4(1.0f), -1.0f * m_currentCamera.position), glm::vec3(
            1.0f / (m_currentCamera.scale * m_currentCamera.resolution.x),
            1.0f / (m_currentCamera.scale * m_currentCamera.resolution.y),
            1.0f / (m_currentCamera.scale)
        ));
    }
    bool MapLayer::checkMouse(const float& objX, const float& objY, const float& mouseX, const float& mouseY, const float& epsilonX, const float& epsilonY) const
    {
        float diffX = std::abs(objX - mouseX);
        float diffY = std::abs(objY - mouseY);
        //LOG_INFO((diffX * diffX / epsilonX / epsilonX + diffY * diffY / epsilonY / epsilonY));
        //return (diffX * diffX / epsilonX / epsilonX + diffY * diffY / epsilonY / epsilonY) < 1;
        return (diffX < epsilonX) && (diffY < epsilonY);
    }
    const std::vector<entt::entity>& MapLayer::getObjectsUnderMouse() const
    {
        return m_objectsUnderMouse;
    }
}
