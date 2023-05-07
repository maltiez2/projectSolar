#include "pch.h"

#include "Application.h"
#include "Layers.h"

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

    Renderer centralRenderer;
    GuiWindowsManager guiWindows;
    
    LayersManager layers;
    layers.add<MapLayer>(1, true, &centralRenderer, &m_simulation);
    layers.add<GuiLayer>(2, true, &window, &guiWindows);

    // *** GUI ***
    guiWindows.add<NotificationWindow>("test", true, "Test window", "Test text of test window");
    guiWindows.add<DebugWindow>("debug", true);
    guiWindows.add<DemoWindow>("demo", false);
    guiWindows.add<PropulsionControlWindow>("prop", true);

    const auto& guiWindow   = *guiWindows.get<NotificationWindow>("test");
    const auto& debugWindow = *guiWindows.get<DebugWindow>("debug");
          auto& demoWindow  = *guiWindows.get<DemoWindow>("demo");
    const auto& propWindow  = *guiWindows.get<PropulsionControlWindow>("prop");
    // ***********

    Eigen::Vector3d forceDirection(1.0, 0.0, 0.0);
    Eigen::Vector3d rotationAxis(0.0, 0.0, 1.0);
    float forceMagnitude = 1.0f;

    while (window.startFrame())
    {
        glfwPollEvents();

        // *** Data setup ***

        auto& player = m_simulation.getData().propulsedData.getData()[0];
        Eigen::AngleAxisd rotation(propWindow.direction, rotationAxis);
        player.propulsion = rotation * forceDirection * forceMagnitude * propWindow.magnitude;

        if (debugWindow.runSimulation)
        {
            m_simulation.run({ 10.0f, 5e-2 * debugWindow.timeScale, 0.3f, 60, 10, -0.1f});
        }

        // *** Central map setup ***

        centralRenderer.clear();

        float scale = 0.05f * debugWindow.scale;
        glm::mat4 proj = glm::ortho(-1.0f * scale * (float)window.getWidth(), 1.0f * scale * (float)window.getWidth(), -1.0f * scale * (float)window.getWidth(), 1.0f * scale * (float)window.getHeight(), -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-player.position[0], -player.position[1], 0));
        if (!propWindow.followPlayer)
        {
            view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        }
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        glm::mat4 mvp = proj * view * model;

        layers.get<MapLayer>(1)->setMVP(mvp);

        // *** GUI setup ***

        demoWindow.showFlag = debugWindow.showDemoWindow;

        // *** Draw ***
        
        layers.draw();

        if (debugWindow.closeApp)
        {
            break;
        }
    }
}

