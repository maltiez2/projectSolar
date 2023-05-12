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
#include "GameLogic/CommunicationManager.h"


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

    Application::Application(std::shared_ptr<Simulation::SimulationRunner> simulation, std::shared_ptr<ECS::EntityManager> entities) :
        m_simulation(simulation),
        m_enitites(entities)
    {
        m_eventHandler = std::make_shared<ApplicationEventHandler>(*this);
        m_window = std::make_shared<Graphics::Window>();

        Com::init(16);

        auto centralRenderer = std::make_shared<Graphics::Renderer>();
        auto guiWindows = std::make_shared<Graphics::GuiWindowsManager>();

        auto mapLayer = m_layers.add<Layers::MapLayer>(1, true, centralRenderer, m_simulation);
        auto guiLayer = m_layers.add<Layers::GuiLayer>(2, true, m_window);

        Com::get().Application = m_eventHandler;
        Com::get().ECS         = std::make_shared<ECS::EntityManager>();
        Com::get().simulation  = std::make_shared<GameLogic::SimulationManager>(simulation);
        Com::get().GUI         = std::make_shared<GameLogic::GuiManager>(guiLayer);
        Com::get().Map         = std::make_shared<GameLogic::MapManager>(mapLayer);

        Com::subsribeOnEvent(Com::SIMULATION_UPDATED, Com::get().GUI);
        Com::subsribeOnEvent(Com::GUI_UPDATED, Com::get().GUI);

        mapLayer->setResolution(m_window->getWidth(), m_window->getHeight());
        m_simulationParams = Com::get().simulation->getRunParams();
    }

    void Application::run()
    {
        EMIT_EVENT(SIMULATION_UPDATED);
        EMIT_EVENT(GUI_UPDATED);
        
        while (m_running)
        {
            m_window->startFrame();

            if (m_simulating)
            {
                auto performance = m_simulation->run(m_simulationParams);
                //LOG_DEBUG("[Application] Simulation sub steps: ", performance.subStepsNumber);
                EMIT_EVENT(SIMULATION_UPDATED, performance.secondsPerStep, performance.subStepsNumber);
            }

            m_layers.draw();

            m_window->finishFrame();

            processInputEvents();
        }
    }

    void Application::processInputEvents()
    {
        auto& eventsManager = m_window->getEventsManager();
        while (!eventsManager.isEmpty())
        {
            if (eventsManager.front()->getEventType() == Graphics::InputEventType::WindowClose)
            {
                m_running = false;
                return;
            }
            
            m_layers.onEvent(eventsManager.front());
            eventsManager.pop();
        }
    }

    ApplicationEventHandler::ApplicationEventHandler(Application& app) :
        m_app(app)
    {
    }
    ApplicationEventHandler::~ApplicationEventHandler()
    {
        destroyWorkers();
    }
    void ApplicationEventHandler::processEvent(uint8_t eventType, uint8_t* data)
    {
        switch (eventType)
        {
            EVENTS_DEF_UNKNOWN();
            EVENT_DEF(SET_RUN_SIMULATION);
            {
                m_app.m_simulating = eventData.run;
            }
            EVENT_DEF(CLOSE_WINDOW);
            {
                m_app.m_running = false;
            }
            EVENT_DEF(SET_SIMULATION_STEP);
            {
                m_app.m_simulationParams.stepSize = eventData.stepSize;
            }
            EVENT_DEF(SET_SIMULATION_LOAD);
            {
                m_app.m_simulationParams.framePeriodFactor = eventData.framePeriodFactor;
            }
            EVENT_DEF(SET_SIMULATION_FPS);
            {
                m_app.m_simulationParams.framePeriodFactor = eventData.framesPerSecond;
            }
            EVENTS_DEF_DEFAULT();
            break;
        }
    }
}
