#include "pch.h"

#include "Graphics.h"
#include "Simulation.h"
#include "Application.h"
#include "InputEvents.h"
#include "EventHandler.h"
#include "Layers/Layers.h"
#include "Layers/GuiLayer.h"
#include "Layers/MapLayer.h"
#include "EventManagers/Managers.h"
#include "ECS/EntityComponentSystem.h"
#include "EventManagers/CommunicationManager.h"
#include "EventManagers/ApplicationEventHandler.h"
#include "EventManagers/SavesManager.h"


namespace projectSolar
{
    Application::Application()
    {
        init();
    }

    void Application::init()
    {
        m_eventHandler = std::make_shared<ApplicationEventHandler>(*this);
        m_window = std::make_shared<Graphics::Window>();
        m_layers = std::make_shared<Layers::LayersManager>();

        Com::init(std::thread::hardware_concurrency());

        Com::get().Application = m_eventHandler;
        Com::get().ECS = std::make_shared<ECS::EntityComponentSystem>();

        m_simLayer = m_layers->add<Layers::SimLayer>(SIM_LAYER_ID, Layers::SimLayer::Params(1e-2, 0.5f / 144.0f));
        Com::get().simulation = std::make_shared<EventManagers::SimulationManager>(m_simLayer);

        m_mapLayer = m_layers->add<Layers::MapLayer>(MAP_LAYER_ID);
        Com::get().Map = std::make_shared<EventManagers::MapManager>(m_mapLayer);

        m_guiLayer = m_layers->add<Layers::GuiLayer>(GUI_LAYER_ID, m_window);
        Com::get().GUI = std::make_shared<EventManagers::GuiManager>(m_guiLayer);

        Com::get().Saves = std::make_shared<EventManagers::SavesManager>(m_layers);

        Com::subsribeToEvent(Com::SIMULATION_UPDATED, Com::get().simulation);
        Com::subsribeToEvent(Com::SIMULATION_UPDATED, Com::get().GUI);
        Com::subsribeToEvent(Com::GUI_UPDATED, Com::get().GUI);

        m_mapLayer->setResolution(m_window->getWidth(), m_window->getHeight());
        m_simLayer->setTimeRest(m_simLoad / (float)m_window->getFPS());

        m_layers->detach<Layers::SimLayer>(SIM_LAYER_ID);
    }

    void Application::run()
    {
        EMIT_EVENT(SIMULATION_UPDATED);
        EMIT_EVENT(GUI_UPDATED);
        
        while (m_running)
        {
            m_window->startFrame();

            m_layers->process();

            m_window->finishFrame();

            processInputEvents();
            processAppState();
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
            
            m_layers->onEvent(eventsManager.front());
            eventsManager.pop();
        }
    }
    void Application::processAppState()
    {
        if (m_simAttached && !m_layers->ifAttached(Application::SIM_LAYER_ID))
        {
            m_layers->reattach<Layers::SimLayer>(Application::SIM_LAYER_ID);
            return;
        }

        if (!m_simAttached && m_layers->ifAttached(Application::SIM_LAYER_ID))
        {
            m_layers->detach<Layers::SimLayer>(Application::SIM_LAYER_ID);
            return;
        }
    }
}
