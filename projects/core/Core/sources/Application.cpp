#include "GraphicsCore.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Application.h"

#include "GUIRenderer.h"
#include "CentralRenderer.h"
#include "SimulationRunner.h"
#include "Logger.h"

#include <string>
#include <vector>
#include <Eigen/Eigen>


using namespace projectSolar;

struct Point
{
    float x;
    float y;
    float z;
    uint32_t type;
};

Application::Application(Simulation::SimulationRunner& simulation) :
    m_simulation(simulation)
{
}

class MapRenderer
{
public:
    explicit MapRenderer(Renderer& centralRenderer, Simulation::SimulationRunner& simulation) :
        m_centralRenderer(centralRenderer),
        m_simulation(simulation),
        m_shader(shaderFile)
    {
        updateData();
        
        m_vertexBuffer = new VertexBuffer(std::to_address(m_buffer.begin()), m_buffer.size() * sizeof(struct Point));
        m_layout.push<float>(3);
        m_layout.push<uint32_t>(1);
        m_vertexArray.addBufer(*m_vertexBuffer, m_layout);
        m_indexBuffer = new IndexBuffer(std::to_address(m_indices.begin()), m_indices.size());
    }
    ~MapRenderer()
    {
        delete(m_vertexBuffer);
        delete(m_indexBuffer);
    }

    void render(glm::mat4 mvp)
    {
        updateData();

        m_vertexBuffer->updateData(std::to_address(m_buffer.begin()), m_buffer.size() * sizeof(struct Point));
        
        // Camera setup
        
        m_shader.bind();
        m_shader.setUniformMat4f("u_MVP", mvp);

        // Render
        m_centralRenderer.clear();
        m_centralRenderer.draw(m_vertexArray, *m_indexBuffer, m_shader);

        // Unbind all
        m_vertexArray.unbind();
        m_shader.unbind();
        m_vertexBuffer->unbind();
        m_indexBuffer->unbind();
    }

private:
    struct Point
    {
        float x;
        float y;
        float z;
        uint32_t type;
    };

    const char* shaderFile = "resources/shaders/map.shader";

    Renderer& m_centralRenderer;
    Simulation::SimulationRunner& m_simulation;
    Shader m_shader;
    VertexBufferLayout m_layout;
    VertexArray m_vertexArray;
    std::vector<Point> m_buffer = {};
    std::vector<uint32_t> m_indices = {};
    
    VertexBuffer* m_vertexBuffer;
    IndexBuffer* m_indexBuffer;

    void updateData()
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
            LOG_DEBUG("Attractor at: \t" + std::to_string(element.position[0]) + "\t" + std::to_string(element.position[1]));
        }

        auto& atantData = dataManager.attractantsData.getData();
        for (size_t i = 0; i < atantData.size(); i++)
        {
            auto& element = atantData[i];
            m_buffer[attractantsIndex + i] = Point(element.position[0], element.position[1], element.position[2], 1);
            LOG_DEBUG("Attractant at: \t" + std::to_string(element.position[0]) + "\t" + std::to_string(element.position[1]));
        }

        auto& propData = dataManager.propulsedData.getData();
        for (size_t i = 0; i < propData.size(); i++)
        {
            auto& element = propData[i];
            m_buffer[propulsesIndex + i] = Point(element.position[0], element.position[1], element.position[2], 2);
            LOG_DEBUG("Propulsed at: \t" + std::to_string(element.position[0]) + "\t" + std::to_string(element.position[1]));
        }

    }
};

void Application::run()
{
    LOG_INTT_CONSOLE("logs/log_imGuiTest.txt");

    Window window;

    // *** Central ***

    Renderer centralRenderer;
    MapRenderer mapRenderer(centralRenderer, m_simulation);

    // *** GUI ***
    auto guiRenderer = GuiRenderer(&window);

    auto guiWindow = NotificationWindow("Test window", "Test text of test window");
    auto debugWindow = DebugWindow();
    auto demoWindow = DemoWindow();
    auto propWindow = PropulsionControlWindow();

    guiRenderer.addGuiWindow("test", &guiWindow, true);
    guiRenderer.addGuiWindow("debug", &debugWindow, true);
    guiRenderer.addGuiWindow("demo", &demoWindow, true);
    guiRenderer.addGuiWindow("prop", &propWindow, true);

    Eigen::Vector3d forceDirection(1.0, 0.0, 0.0);
    Eigen::Vector3d rotationAxis(0.0, 0.0, 1.0);
    float forceMagnitude = 1.0f;

    while (!glfwWindowShouldClose((GLFWwindow*)window.getNativeWindow()))
    {
        glfwPollEvents();

        // *** Data ***

        auto& player = m_simulation.getData().propulsedData.getData()[0];
        Eigen::AngleAxisd rotation(propWindow.direction, rotationAxis);
        player.propulsion = rotation * forceDirection * forceMagnitude * propWindow.magnitude;

        if (debugWindow.runSimulation)
        {
            m_simulation.run({ 1.0f, 5e-2 * debugWindow.timeScale, 0.3f, 60, 10, -0.1f});
        }

        // *** Central ***

        float scale = 0.05f * debugWindow.scale;
        glm::mat4 proj = glm::ortho(-1.0f * scale * window.getWidth(), 1.0f * scale * window.getWidth(), -1.0f * scale * window.getWidth(), 1.0f * scale * window.getHeight(), -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-player.position[0], -player.position[1], 0));
        if (!propWindow.followPlayer)
        {
            view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        }
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        glm::mat4 mvp = proj * view * model;

        mapRenderer.render(mvp);

        // *** GUI ***

        demoWindow.showFlag = debugWindow.showDemoWindow;
        guiRenderer.render();

        glfwSwapBuffers((GLFWwindow*)window.getNativeWindow());

        if (debugWindow.closeApp)
        {
            break;
        }
    }
}

