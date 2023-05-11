#include "pch.h"

#include "GuiLayer.h"

namespace projectSolar::Layers
{
    GuiLayer::GuiLayer(std::shared_ptr<Graphics::Window> window, std::shared_ptr<Graphics::GuiWindowsManager> guiWindows, bool blockEvents) :
        m_windowsManager(guiWindows),
        m_renderer(*window, *guiWindows),
        m_blockEvents(blockEvents)
    {
    }

    void GuiLayer::draw()
    {
        m_renderer.render();
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
