#include "GuiWindow.h"

#include "imgui/imgui.h"

#include <utility>

void imGuiTest::GuiWindow::show()
{
    // virtual method
}

imGuiTest::NotificationWindow::NotificationWindow(const std::string& title, const std::string& message) :
	m_title(title),
	m_message(message)
{
}
void imGuiTest::NotificationWindow::show()
{
    ImGui::Begin(m_title.c_str(), &showFlag);
    ImGui::Text(m_message.c_str());
    ImGui::End();
}

void imGuiTest::DebugWindow::show()
{
    ImGui::Begin("Debug", &showFlag);
    ImGui::Checkbox("Demo Window", &showDemoWindow);
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}

void imGuiTest::DemoWindow::show()
{
    ImGui::ShowDemoWindow(&showFlag);
}
