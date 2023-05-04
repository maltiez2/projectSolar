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
    bool show_demo_window;
    const ImGuiIO& io = ImGui::GetIO();
    
    ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::ShowStyleEditor();

    if (false)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    bool mainWindowOpen;
    ImGui::Begin(m_title.c_str(), &mainWindowOpen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove);

    static float f = 0.0f;
    static int counter = 0;

    ImGui::Text(m_message.c_str());
    ImGui::Checkbox("Demo Window", &show_demo_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

    if (ImGui::Button("Button"))
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
    ImGui::PopStyleVar(1);
}


