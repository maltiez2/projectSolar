#include "pch.h"

#include "GuiLayer.h"

#include "Graphics.h"
#include "GuiApi.h"
#include "InputEvents.h"
#include "EventManagers/CommunicationManager.h"


namespace projectSolar::Layers
{
    GuiLayer::GuiLayer(std::shared_ptr<Graphics::Window> window, bool blockEvents) :
        m_blockEvents(blockEvents)
    {
        m_renderer = std::make_shared<Graphics::GuiRenderer>(*window, *m_windowsManager);
    }

    void GuiLayer::process()
    {
        PROFILE_FUNCTION();
        
        m_renderer->render();

        EMIT_EVENT(GUI_DRAWN);
        EMIT_EVENT(GUI_UPDATED);
    }
    void GuiLayer::onEvent(Graphics::InputEvent* ev)
    {
        if (m_blockEvents)
        {
            const ImGuiIO& io = ImGui::GetIO();
            ev->handled |= ev->isInCategory(Graphics::EventCategoryMouse) & io.WantCaptureMouse;
            ev->handled |= ev->isInCategory(Graphics::EventCategoryKeyboard) & io.WantCaptureKeyboard;
        }
    }
    void GuiLayer::save(const std::string& filePath)
    {
        // Nothing to save yet
    }
    void GuiLayer::load(const std::string& filePath)
    {
        // Nothing to load yet
    }
    std::shared_ptr<Graphics::GuiWindowsManager> GuiLayer::getWindowsManager()
    {
        return m_windowsManager;
    }
}
