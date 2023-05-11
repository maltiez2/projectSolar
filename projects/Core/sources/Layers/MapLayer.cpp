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

        m_vertexBuffer->updateData(std::to_address(m_buffer.begin()), m_buffer.size() * sizeof(struct Point));
        m_shader->bind();
        glm::mat4 MVP = m_proj * m_view * m_model;
        m_shader->setUniformMat4f("u_MVP", MVP);

        m_centralRenderer->draw(*m_vertexArray, *m_indexBuffer, *m_shader);

        m_vertexBuffer->unbind();
        m_indexBuffer->unbind();
        m_vertexArray->unbind();
        m_shader->unbind();
    }
    void MapLayer::onEvent(Graphics::InputEvent* ev)
    {
        //LOG_DEBUG("[event] [MapLayer] ", ev->toString());
    }
    void MapLayer::setProj(const glm::mat4& proj)
    {
        m_proj = proj;
    }
    void MapLayer::setModel(const glm::mat4& model)
    {
        m_model = model;
    }
    void MapLayer::setView(const glm::mat4& view)
    {
        m_view = view;
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

    SLOT_IMPL(MapLayer, SET_PROJ)
    {
        LOG_WARN("[MapLayer] SET_PROJ: ", data->width, " : ", data->height, " - ", data->scale);
        float width = data->scale * (float)data->width;
        float height = data->scale * (float)data->height;
        LOG_WARN("[MapLayer] SET_PROJ scaled: ", width, " : ", height);
        m_proj = glm::ortho(-0.5f * width, 0.5f * width, -0.5f * height, 0.5f * height, -1.0f, 1.0f);
    }
    SLOT_IMPL(MapLayer, SET_VIEW)
    {
        LOG_WARN("[MapLayer] SET_VIEW: ", data->x, ", ", data->y, ", ", data->z);
        m_view = glm::translate(glm::mat4(1.0f), glm::vec3(-data->x, -data->y, -data->z));
    }
    SLOT_IMPL(MapLayer, SET_MODEL)
    {
        LOG_WARN("[MapLayer] SET_MODEL: ", data->x, ", ", data->y, ", ", data->z);
        m_model = glm::translate(glm::mat4(1.0f), glm::vec3(data->x, data->y, data->z));
    }
    SLOT_IMPL(MapLayer, TRANSLATE_VIEW)
    {
        LOG_WARN("[MapLayer] TRANSLATE_VIEW: ", data->x, ", ", data->y, ", ", data->z);
        m_view = glm::translate(m_view, glm::vec3(-data->x, -data->y, -data->z));
    }
    SLOT_IMPL(MapLayer, TRANSLATE_MODEL)
    {
        LOG_WARN("[MapLayer] TRANSLATE_MODEL: ", data->x, ", ", data->y, ", ", data->z);
        m_model = glm::translate(m_model, glm::vec3(data->x, data->y, data->z));
    }
}
