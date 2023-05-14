#include "pch.h"

#include "GuiLayer.h"

namespace projectSolar::Layers
{
    GuiLayer::GuiLayer(std::shared_ptr<Graphics::Window> window, bool blockEvents) :
        m_renderer(*window, *m_windowsManager),
        m_blockEvents(blockEvents)
    {

    }

    void GuiLayer::process()
    {
        m_renderer.render();

        EMIT_EVENT(GUI_DRAWN);
        EMIT_EVENT(GUI_UPDATED);
    }
    void GuiLayer::onEvent(Graphics::InputEvent* ev)
    {
        if (m_blockEvents)
        {
            ImGuiIO& io = ImGui::GetIO();
            ev->handled |= ev->isInCategory(Graphics::EventCategoryMouse) & io.WantCaptureMouse;
            ev->handled |= ev->isInCategory(Graphics::EventCategoryKeyboard) & io.WantCaptureKeyboard;
        }
    }
    std::shared_ptr<Graphics::GuiWindowsManager> GuiLayer::getWindowsManager()
    {
        return m_windowsManager;
    }
}
