#include "pch.h"

#include "Graphics.h"
#include "Simulation.h"
#include "Application.h"
#include "Layers.h"

#include <string>
#include <vector>
#include <Eigen/Eigen>
#include <functional>

#include "Tasks/TaskManager.h"
#include "Events/EventHandler.h"


using namespace projectSolar;

bool comparePositions(const Eigen::Vector3d& p1, const Eigen::Vector3d& p2, const double& epsilon)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        if (std::abs(p1[i] - p2[i]) > std::abs(p1[i]) * epsilon)
        {
            return false;
        }
    }

    return true;
}

struct Point
{
    float x;
    float y;
    float z;
    uint32_t type;
};

Application::Application(Simulation::SimulationRunner& simulation, const WindowProperties& windowProps) :
    EventHandler(1),
    m_simulation(simulation),
    m_window(new Window(windowProps))
{
}

Application::~Application()
{
    delete(m_window);
}

void Application::run()
{
    
    
    Renderer centralRenderer;
    GuiWindowsManager guiWindows;
    
    //TaskManager taskManager(&m_simulation, &guiWindows, m_window);
    
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
    Eigen::Vector3d playerPosition = m_simulation.getData().propulsedData.getData()[0].position;
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
            m_simulation.run({ 10.0f, 5e-2 * debugWindow.timeScale, 0.5f, 144, 10, -0.1f});
        }

        // *** Central map setup ***

        float scale = 0.05f * debugWindow.scale;
        glm::mat4 proj = glm::ortho(-1.0f * scale * (float)m_window->getWidth(), 1.0f * scale * (float)m_window->getWidth(), -1.0f * scale * (float)m_window->getWidth(), 1.0f * scale * (float)m_window->getHeight(), -1.0f, 1.0f);
        m_layers.get<MapLayer>(1)->setProj(proj);
        
        if (propWindow.followPlayer && !comparePositions(playerPosition, player.position, 1e-5))
        {
            //EMIT_EVENT(&m_handler, APP_SET_MAP_VIEW, 1, (float)player.position[0], (float)player.position[1], (float)player.position[2]);
            playerPosition = player.position;
        }
 

        // *** GUI setup ***

        demoWindow.showFlag = debugWindow.showDemoWindow;

        // *** Draw ***
        
        m_layers.draw();

        if (debugWindow.closeApp)
        {
            //m_running = false;
            EMIT_EVENT(Application, this, DEBUG_MESSAGE, "test message");
            EMIT_EVENT(Application, this, CLOSE_WINDOW);
        }

        processInputEvents();

        //taskManager.execute();

        //handler.send(&handler, Command::WND_WINDOW_CLOSE, nullptr);

        m_running = !(bool)glfwWindowShouldClose(m_window->getNativeWindow());
    }
}

void Application::processInputEvents()
{
    auto& eventsManager = m_window->getEventsManager();
    while (!eventsManager.isEmpty())
    {
        m_layers.onEvent(eventsManager.front());
        eventsManager.pop();
    }
}

void projectSolar::Application::processEvent(const Events::Event& ev)
{
    switch (ev.command)
    {
    case CLOSE_WINDOW:
        return PROCESS_EVENT(ev.sender, CLOSE_WINDOW, ev.data);
    case DEBUG_MESSAGE:
        return PROCESS_EVENT(ev.sender, DEBUG_MESSAGE, ev.data);
    default:
        break;
    }
}

