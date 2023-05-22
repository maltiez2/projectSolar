#include "pch.h"

#include "Simulation.h"
#include "EventHandler.h"
#include "ApplicationEventHandler.h"
#include "Application.h"
#include "Logger.h"

namespace projectSolar
{
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
            EVENT_DEF(SET_SIM_LOAD);
            {
                m_app.m_simLoad = eventData.load;
            }
            EVENTS_DEF_DEFAULT();
            break;
        }
    }
}

