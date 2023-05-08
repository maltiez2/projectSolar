#include "pch.h"

#include "Layers.h"

using namespace projectSolar;

projectSolar::LayersManager::~LayersManager()
{
	for (auto& pointer : m_layers)
	{
		delete(pointer);
	}
}
void projectSolar::LayersManager::draw()
{
    for (auto& [id, layer] : m_attached)
    {
        layer->draw();
    }
}
bool projectSolar::LayersManager::onEvent(InputEvent* ev)
{
    for (auto it = m_attached.rbegin(); it != m_attached.rend(); ++it)
    {
        it->second->onEvent(ev);
        if (ev->handled)
        {
            return true;
        }
    }
    return false;
}
bool projectSolar::LayersManager::attach(const std::size_t& id, Layer* layer)
{
    if (m_attached.contains(id));
    {
        LOG_ERROR("[LayersManager][attach()] Layer with id '", id, "' already exists");
        return false;
    }

    m_attached.try_emplace(id, layer);
    
    return true;
}
Layer* projectSolar::LayersManager::detach(const std::size_t& id)
{
    if (!m_attached.contains(id))
    {
        LOG_ERROR("[LayersManager][detach()] Layer with id '", id, "' does not exist");
        return nullptr;
    }
    
    auto* layer = m_attached.at(id);

    m_attached.erase(id);

    return layer;
}


MapLayer::MapLayer(Renderer* centralRenderer, Simulation::SimulationRunner* simulation) :
    m_centralRenderer(*centralRenderer),
    m_simulation(*simulation),
    m_shader(shaderFile)
{
    m_proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    m_model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    m_view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    
    updateData();

    m_vertexBuffer = new VertexBuffer(std::to_address(m_buffer.begin()), m_buffer.size() * sizeof(struct Point));
    m_layout.push<float>(3);
    m_layout.push<uint32_t>(1);
    m_vertexArray.addBufer(*m_vertexBuffer, m_layout);
    m_indexBuffer = new IndexBuffer(std::to_address(m_indices.begin()), m_indices.size());
}
MapLayer::~MapLayer()
{
    delete(m_vertexBuffer);
    delete(m_indexBuffer);
}
void MapLayer::draw()
{
    updateData();

    m_vertexBuffer->updateData(std::to_address(m_buffer.begin()), m_buffer.size() * sizeof(struct Point));
    m_shader.bind();
    glm::mat4 MVP = m_proj * m_view * m_model;
    m_shader.setUniformMat4f("u_MVP", MVP);

    m_centralRenderer.draw(m_vertexArray, *m_indexBuffer, m_shader);

    m_vertexArray.unbind();
    m_shader.unbind();
    m_vertexBuffer->unbind();
    m_indexBuffer->unbind();
}
void projectSolar::MapLayer::onEvent(InputEvent* ev)
{
    LOG_DEBUG("[event] [MapLayer] ", ev->toString());
}
void projectSolar::MapLayer::setMVP()
{
    m_shader.bind();
    m_shader.setUniformMat4f("u_MVP", m_proj * m_view * m_model);
    m_shader.unbind();
}
void projectSolar::MapLayer::setProj(const glm::mat4& proj)
{
    m_proj = proj;
}
void projectSolar::MapLayer::setModel(const glm::mat4& model)
{
    m_model = model;
}
void projectSolar::MapLayer::setView(const glm::mat4& view)
{
    m_view = view;
}
void MapLayer::updateData()
{
    auto& dataManager = m_simulation.getData();

    size_t attractantsIndex = dataManager.attractorsData.size();
    size_t propulsesIndex = dataManager.attractantsData.size() + attractantsIndex;
    size_t bufferSize = dataManager.propulsedData.size() + propulsesIndex;

    m_buffer.resize(bufferSize);
    m_indices.resize(bufferSize);

    for (size_t i = 0; i < m_indices.size(); i++)
    {
        m_indices[i] = i;
    }

    auto& atorData = dataManager.attractorsData.getData();
    for (size_t i = 0; i < atorData.size(); i++)
    {
        auto& element = atorData[i];
        m_buffer[i] = Point(element.position[0], element.position[1], element.position[2], 0);
    }

    auto& atantData = dataManager.attractantsData.getData();
    for (size_t i = 0; i < atantData.size(); i++)
    {
        auto& element = atantData[i];
        m_buffer[attractantsIndex + i] = Point(element.position[0], element.position[1], element.position[2], 1);

    }

    auto& propData = dataManager.propulsedData.getData();
    for (size_t i = 0; i < propData.size(); i++)
    {
        auto& element = propData[i];
        m_buffer[propulsesIndex + i] = Point(element.position[0], element.position[1], element.position[2], 2);
    }
}

projectSolar::GuiLayer::GuiLayer(Window* window, GuiWindowsManager* guiWindows, bool blockEvents) :
    m_renderer(*window, *guiWindows),
    m_blockEvents(blockEvents)
{
}

void projectSolar::GuiLayer::draw()
{
    m_renderer.render();
}
void projectSolar::GuiLayer::onEvent(InputEvent* ev)
{
    if (m_blockEvents)
    {
        LOG_DEBUG("[event] [GuiLayer] ", ev->toString());

        ImGuiIO& io = ImGui::GetIO();
        ev->handled |= ev->isInCategory(EventCategoryMouse) & io.WantCaptureMouse;
        ev->handled |= ev->isInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
    }
    
}
