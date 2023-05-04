#include "GraphicsCore.h"

#include "GuiWindow.h"

#include <utility>

projectSolar::NotificationWindow::NotificationWindow(const std::string& title, const std::string& message) :
	m_title(title),
	m_message(message)
{
}
void projectSolar::NotificationWindow::show()
{
    ImGui::Begin(m_title.c_str(), &showFlag);
    ImGui::Text(m_message.c_str());
    ImGui::End();
}

void projectSolar::DebugWindow::show()
{
    ImGui::Begin("Debug", &showFlag);
    ImGui::Checkbox("Demo Window", &showDemoWindow);
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}

void projectSolar::DemoWindow::show()
{
    ImGui::ShowDemoWindow(&showFlag);
}
