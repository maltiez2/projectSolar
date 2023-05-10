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
#include "EventHandler.h"
#include "SubscriptionManager.h"


namespace projectSolar
{

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

    Application::Application(Simulation::SimulationRunner& simulation, const Graphics::WindowProperties& windowProps) :
        EventHandler(1),
        m_simulation(simulation),
        m_window(new Graphics::Window(windowProps))
    {
        projectSolar::SubscriptionManager::init(16);
    }

    Application::~Application()
    {
        delete(m_window);
    }

    void Application::run()
    {
        SUBSCRIBE(Application, this, DEBUG_MESSAGE, DEBUG_MESSAGE);

        Graphics::Renderer centralRenderer;
        Graphics::GuiWindowsManager guiWindows;

        MapLayer* mapLayer = (MapLayer*)m_layers.add<MapLayer>(1, true, &centralRenderer, &m_simulation);
        m_layers.add<GuiLayer>(2, true, m_window, &guiWindows);

        // *** GUI ***
        guiWindows.add<Graphics::NotificationWindow>("test", true, "Test window", "Test text of test window");
        guiWindows.add<Graphics::DebugWindow>("debug", true);
        guiWindows.add<Graphics::DemoWindow>("demo", false);
        guiWindows.add<Graphics::PropulsionControlWindow>("prop", true);

        const auto& guiWindow = *guiWindows.get<Graphics::NotificationWindow>("test");
        const auto& debugWindow = *guiWindows.get<Graphics::DebugWindow>("debug");
        auto& demoWindow = *guiWindows.get<Graphics::DemoWindow>("demo");
        const auto& propWindow = *guiWindows.get<Graphics::PropulsionControlWindow>("prop");
        // ***********

        Eigen::Vector3d forceDirection(1.0, 0.0, 0.0);
        Eigen::Vector3d rotationAxis(0.0, 0.0, 1.0);
        Eigen::Vector3d playerPosition = m_simulation.getData().propulsedData.getData()[0].position;
        const float scaleMult = 0.05f;
        const float epsilon = 1e-5f;
        float scale = 0.0f;
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
                m_simulation.run({ 10.0f, 5e-2 * debugWindow.timeScale, 0.5f, 144, 10, -0.1f });
            }

            // *** Central map setup ***

            if (std::abs(debugWindow.scale - scale) > scale * epsilon)
            {
                scale = debugWindow.scale;
                SEND_EVENT(MapLayer, mapLayer, SET_PROJ, m_window->getWidth(), m_window->getHeight(), scale * scaleMult);
            }

            if (propWindow.followPlayer && !comparePositions(playerPosition, player.position, epsilon))
            {
                SEND_EVENT(MapLayer, mapLayer, SET_VIEW, (float)player.position[0], (float)player.position[1], (float)player.position[2]);
                playerPosition = player.position;
            }


            // *** GUI setup ***

            demoWindow.showFlag = debugWindow.showDemoWindow;

            // *** Draw ***

            m_layers.draw();

            // *** Test ***

            
            
            if (debugWindow.closeApp)
            {  
                SEND_EVENT(Application, this, DEBUG_MESSAGE, "test msg");
                for (int i = 0; i < 1e5; i++)
                {
                    EMIT_EVENT(DEBUG_MESSAGE, "debug message event #" + std::to_string(i));
                }
            }

            processInputEvents();

            if (glfwWindowShouldClose(m_window->getNativeWindow()))
            {
                m_running = false;
            }
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

    SLOT_IMPL(Application, CLOSE_WINDOW)
    {
        LOG_DEBUG("CLOSE_WINDOW");
        m_running = false;
    }
    SLOT_IMPL(Application, DEBUG_MESSAGE)
    {
        //LOG_INFO("DEBUG_MESSAGE: ", data->message);
        //return;
        
        static std::atomic<int> counter;
        counter++;
        int copy = counter;
        if (copy % 1000 == 0)
        {
            LOG_INFO("DEBUG_MESSAGE #", copy);
        }
    }
}
