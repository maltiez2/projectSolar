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
                auto perf = ObM::get().managerSimulation->run();
                LOG_INFO("[Application] Simulation performance - milliseconds per step: ", 1e3f * perf.secondsPerStep, ", substeps per step: ", perf.subStepsNumber);
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
                m_running = false;
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
}
