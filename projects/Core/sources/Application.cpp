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

    Application::Application(std::shared_ptr<ECS::EntityManager> entities) :
        m_enitites(entities)
    {
        m_eventHandler = std::make_shared<ApplicationEventHandler>(*this);
        m_window = std::make_shared<Graphics::Window>();

        Com::init(16);

        auto centralRenderer = std::make_shared<Graphics::Renderer>();
        auto guiWindows = std::make_shared<Graphics::GuiWindowsManager>();

        Com::get().Application = m_eventHandler;
        Com::get().ECS = std::make_shared<ECS::EntityManager>();

        m_simLayer = m_layers.add<Layers::SimLayer>(SIM_LAYER_ID, Layers::SimLayer::Params(1e-2, 0.5 / 144.0));
        Com::get().simulation = std::make_shared<GameLogic::SimulationManager>(m_simLayer);

        m_mapLayer = m_layers.add<Layers::MapLayer>(MAP_LAYER_ID, centralRenderer);
        Com::get().Map = std::make_shared<GameLogic::MapManager>(m_mapLayer);

        m_guiLayer = m_layers.add<Layers::GuiLayer>(GUI_LAYER_ID, m_window);
        Com::get().GUI = std::make_shared<GameLogic::GuiManager>(m_guiLayer);

        Com::subsribeOnEvent(Com::SIMULATION_UPDATED, Com::get().GUI);
        Com::subsribeOnEvent(Com::GUI_UPDATED, Com::get().GUI);

        m_mapLayer->setResolution(m_window->getWidth(), m_window->getHeight());
        m_simLayer->setTimeRest(m_simLoad / (float)m_window->getFPS());

        m_layers.detach<Layers::SimLayer>(SIM_LAYER_ID);
    }

    void Application::run()
    {
        EMIT_EVENT(SIMULATION_UPDATED);
        EMIT_EVENT(GUI_UPDATED);
        
        // Temporary
        size_t dataSize = m_simLayer->get<Simulation::Motion>(GameLogic::SimulationManager::MOTION_SIM)->data.size();
        m_simLayer->setSimOrder(GameLogic::SimulationManager::MOTION_SIM, { { 0, dataSize - 1 } });
        m_simLayer->setSimOrder(GameLogic::SimulationManager::GRAVITY_SIM, { { 0, dataSize - 1 } });
        
        while (m_running)
        {
            m_window->startFrame();

            m_layers.process();

            m_window->finishFrame();

            processInputEvents();
            processAppCondition();
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
            if (eventsManager.front()->getEventType() == Graphics::InputEventType::WindowResize)
            {
                uint32_t currentFPS = m_window->getFPS();
                if (m_targetFPS != currentFPS)
                {
                    m_targetFPS = currentFPS;
                    m_simLayer->setTimeRest(m_simLoad / (float)currentFPS);
                }
            }
            
            m_layers.onEvent(eventsManager.front());
            eventsManager.pop();
        }
    }
    void Application::processAppCondition()
    {
        if (m_simAttached && !m_layers.ifAttached(Application::SIM_LAYER_ID))
        {
            m_layers.attach(Application::SIM_LAYER_ID, m_simLayer);
            return;
        }

        if (!m_simAttached && m_layers.ifAttached(Application::SIM_LAYER_ID))
        {
            m_simLayer = m_layers.detach<Layers::SimLayer>(Application::SIM_LAYER_ID);
            return;
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
                m_app.m_simAttached = eventData.run;
            }
            EVENT_DEF(CLOSE_WINDOW);
            {
                m_app.m_running = false;
            }
            EVENTS_DEF_DEFAULT();
            break;
        }
    }



    Simulation::DoubleBuffVector<Simulation::Motion::Data>& Application::DEBUG_getSimData()
    {
        return m_simLayer->get<Simulation::Motion>(GameLogic::SimulationManager::MOTION_SIM)->data;
    }
}
