#include "pch.h"

#include "Graphics.h"
#include "Simulation.h"
#include "Application.h"
#include "Layers.h"

#include <string>
#include <vector>
#include <Eigen/Eigen>
#include <functional>


using namespace projectSolar;

struct Point
{
    float x;
    float y;
    float z;
    uint32_t type;
};

Application::Application(Simulation::SimulationRunner& simulation, const WindowProperties& windowProps) :
    m_simulation(simulation),
    m_window(new Window(windowProps)),
    m_layers()
{
    m_window->setEventCallback(BIND_EVENT_FUNC(Application::onEvent));
}

projectSolar::Application::~Application()
{
    delete(m_window);
}

void Application::run()
{
    LOG_INTT_CONSOLE("logs/log_imGuiTest.txt");

    Renderer centralRenderer;
    GuiWindowsManager guiWindows;
    
    m_layers.add<MapLayer>(1, true, &centralRenderer, &m_simulation);
    m_layers.add<GuiLayer>(2, true, m_window, &guiWindows);

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

    while (m_running)
    {
        m_window->startFrame();

        // *** Data setup ***

        auto& player = m_simulation.getData().propulsedData.getData()[0];
        Eigen::AngleAxisd rotation(propWindow.direction, rotationAxis);
        player.propulsion = rotation * forceDirection * forceMagnitude * propWindow.magnitude;

        if (debugWindow.runSimulation)
        {
            m_simulation.run({ 10.0f, 5e-2 * debugWindow.timeScale, 0.3f, 60, 10, -0.1f});
        }

        // *** Central map setup ***

        float scale = 0.05f * debugWindow.scale;
        glm::mat4 proj = glm::ortho(-1.0f * scale * (float)m_window->getWidth(), 1.0f * scale * (float)m_window->getWidth(), -1.0f * scale * (float)m_window->getWidth(), 1.0f * scale * (float)m_window->getHeight(), -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-player.position[0], -player.position[1], 0));
        if (!propWindow.followPlayer)
        {
            view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        }
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        glm::mat4 mvp = proj * view * model;

        m_layers.get<MapLayer>(1)->setMVP(mvp);

        // *** GUI setup ***

        demoWindow.showFlag = debugWindow.showDemoWindow;

        // *** Draw ***
        
        m_layers.draw();

        if (debugWindow.closeApp)
        {
            m_running = false;
        }
    }
}

void projectSolar::Application::onEvent(Event& ev)
{
    EventDispatcher dispatcher(ev);

    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::onWindowClose)); // @TODO add here resize event also

    m_layers.onEvent(ev);
}

bool projectSolar::Application::onWindowClose(WindowCloseEvent& ev)
{
    m_running = false;
    
    return true;
}

