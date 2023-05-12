#include "pch.h"

#include "GuiWindow.h"


using namespace projectSolar::Graphics;

NotificationWindow::NotificationWindow(const std::string& title, const std::string& message) :
	m_title(title),
	m_message(message)
{
}
void NotificationWindow::show()
{
    ImGui::Begin(m_title.c_str(), &showFlag);
    ImGui::Text(m_message.c_str());
    ImGui::End();
}

void DebugWindow::show()
{
    ImGui::Begin("Debug", &showFlag);
    ImGui::SliderFloat("Scale", &scale, 0.0f, 10.0f);
    ImGui::SliderFloat("Time scale", &timeScale, -10.0f, 10.0f);
    ImGui::Checkbox("Run simulation", &runSimulation);
    ImGui::Checkbox("Demo Window", &showDemoWindow);
    closeApp = ImGui::Button("Close app");
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}

void DemoWindow::show()
{
    ImGui::ShowDemoWindow(&showFlag);
}
