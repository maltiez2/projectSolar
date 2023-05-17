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

void DemoWindow::show()
{
    ImGui::ShowDemoWindow(&showFlag);
}
