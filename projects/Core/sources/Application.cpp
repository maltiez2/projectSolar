#include "pch.h"

#include "Graphics.h"
#include "Simulation.h"
#include "Application.h"
#include "Layers/Layers.h"
#include "Layers/GuiLayer.h"
#include "Layers/MapLayer.h"
#include "GameLogic/Managers.h"

#include <string>
#include <vector>
#include <Eigen/Eigen>
#include <functional>

#include "EventHandler.h"
#include "SubscriptionManager.h"
#include "ObjectManager.h"


namespace projectSolar
{
    using ObM = ObjectManager;

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

    Application::Application(std::shared_ptr<Simulation::SimulationRunner> simulation, std::shared_ptr<ECS::EntityManager> entities) :
        EventHandler(8),
        m_simulation(simulation),
        m_enitites(entities)
    {
        m_window = std::make_shared<Graphics::Window>();

        ObjectManager::init();

        auto centralRenderer = std::make_shared<Graphics::Renderer>();
        auto guiWindows = std::make_shared<Graphics::GuiWindowsManager>();

        ObM::get().layerMap = m_layers.add<Layers::MapLayer>(1, true, centralRenderer, m_simulation);
        ObM::get().layerGui = m_layers.add<Layers::GuiLayer>(2, true, m_window, guiWindows);

        ObM::get().managerECS = std::make_shared<ECS::EntityManager>();
        ObM::get().managerSimulation = std::make_shared<GameLogic::SimulationManager>(simulation);
    }

    void Application::run()
    {
        // *** GUI ***
        ObM::get().layerGui->getWindowsManager()->add<Graphics::NotificationWindow>("test", true, "Test window", "Test text of test window");
        ObM::get().layerGui->getWindowsManager()->add<Graphics::DebugWindow>("debug", true);
        ObM::get().layerGui->getWindowsManager()->add<Graphics::DemoWindow>("demo", false);
        ObM::get().layerGui->getWindowsManager()->add<Graphics::PropulsionControlWindow>("prop", true);

        const auto& guiWindow   = *ObM::get().layerGui->getWindowsManager()->get<Graphics::NotificationWindow>("test");
        const auto& debugWindow = *ObM::get().layerGui->getWindowsManager()->get<Graphics::DebugWindow>("debug");
              auto& demoWindow  = *ObM::get().layerGui->getWindowsManager()->get<Graphics::DemoWindow>("demo");
        const auto& propWindow  = *ObM::get().layerGui->getWindowsManager()->get<Graphics::PropulsionControlWindow>("prop");
        // ***********

        Eigen::Vector3d forceDirection(1.0, 0.0, 0.0);
        Eigen::Vector3d rotationAxis(0.0, 0.0, 1.0);
        Eigen::Vector3d playerPosition = m_simulation->getData().propulsedData.getData()[0].position;
        const float scaleMult = 0.05f;
        const float epsilon = 1e-5f;
              float scale = 0.0f;
              float forceMagnitude = 1.0f;

        while (m_running)
        {
            m_window->startFrame();

            // *** Data setup ***

            auto& player = m_simulation->getData().propulsedData.getData()[0];
            Eigen::AngleAxisd rotation(propWindow.direction, rotationAxis);
            player.propulsion = rotation * forceDirection * forceMagnitude * propWindow.magnitude;

            if (debugWindow.runSimulation)
            {
                ObM::get().managerSimulation->run();
                //m_simulation->run({ 10.0f, 5e-2 * debugWindow.timeScale, 0.5f, 144, 10, -0.1f });
                auto perf = ObM::get().managerSimulation->getPerformance();
                std::string mess = "SPS: " + std::to_string(perf.stepsPerSecond) + ", subSteps: " + std::to_string(perf.subStepsNumber);
                SEND_EVENT(Application, this, DEBUG_MESSAGE, mess);
            }

            // *** Central map setup ***


            float scale = 0.05f * debugWindow.scale;
            glm::mat4 proj = glm::ortho(-1.0f * scale * (float)m_window->getWidth(), 1.0f * scale * (float)m_window->getWidth(), -1.0f * scale * (float)m_window->getHeight(), 1.0f * scale * (float)m_window->getHeight(), -1.0f, 1.0f);
            glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-player.position[0], -player.position[1], 0));
            if (!propWindow.followPlayer)
            {
                view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
            }
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
            
            ObM::get().layerMap->setProj(proj);
            ObM::get().layerMap->setView(view);
            ObM::get().layerMap->setModel(model);


            // *** GUI setup ***

            demoWindow.showFlag = debugWindow.showDemoWindow;

            // *** Draw ***

            m_layers.draw();

            // *** Test ***

            
            
            if (debugWindow.closeApp)
            {  
                SEND_EVENT(Application, this, CLOSE_WINDOW);
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
        m_running = false;
    }
    SLOT_IMPL(Application, DEBUG_MESSAGE)
    {
        LOG_INFO("DEBUG_MESSAGE: ", data->message);
    }
}
