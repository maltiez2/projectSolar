#include "GraphicsCore.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Application.h"

#include "GUIRenderer.h"
#include "CentralRenderer.h"
#include "SimulationRunner.h"
#include "MapLayer.h"
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

void Application::run()
{
    LOG_INTT_CONSOLE("logs/log_imGuiTest.txt");

    Window window;

    // *** Central ***

    Renderer centralRenderer;
    MapLayer mapLayer(centralRenderer, m_simulation);

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

        mapLayer.draw(mvp);

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

