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
        m_simulation = std::make_shared<Simulation::SimulationRunner>();
        m_window = std::make_shared<Graphics::Window>();
        m_layers = std::make_shared<Layers::LayersManager>();

        Com::init(std::thread::hardware_concurrency());

        Com::get().Application = m_eventHandler;
        Com::get().ECS = std::make_shared<ECS::EntityComponentSystem>();

        m_mapLayer = m_layers->add<Layers::MapLayer>(MAP_LAYER_ID);
        Com::get().Map = std::make_shared<EventManagers::MapManager>(m_mapLayer);

        m_guiLayer = m_layers->add<Layers::GuiLayer>(GUI_LAYER_ID, m_window);
        Com::get().GUI = std::make_shared<EventManagers::GuiManager>(m_guiLayer);

        Com::get().Saves = std::make_shared<EventManagers::SavesManager>(m_layers);

        Com::subsribeToEvent(Com::SIMULATION_UPDATED, Com::get().simulation);
        Com::subsribeToEvent(Com::SIMULATION_UPDATED, Com::get().GUI);
        Com::subsribeToEvent(Com::GUI_UPDATED, Com::get().GUI);

        m_mapLayer->setResolution(m_window->getWidth(), m_window->getHeight());
    }

    void Application::run()
    {
        EMIT_EVENT(SIMULATION_UPDATED);
        EMIT_EVENT(GUI_UPDATED);
        
        while (m_running)
        {
            PROFILE_SCOPE("Application run cycle");
            
            m_window->startFrame();

            m_layers->process();

            m_window->finishFrame();

            processInputEvents();
        }
    }

    void Application::processInputEvents()
    {
        PROFILE_FUNCTION();
        auto& eventsManager = m_window->getEventsManager();
        while (!eventsManager.isEmpty())
        {
            if (eventsManager.front()->getEventType() == Graphics::InputEventType::WindowClose)
            {
                m_running = false;
                return;
            }
            
            m_layers->onEvent(eventsManager.front());
            eventsManager.pop();
        }
    }
}
