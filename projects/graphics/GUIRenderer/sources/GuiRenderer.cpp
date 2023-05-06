#include "GraphicsCore.h"

#include "GuiRenderer.h"

projectSolar::GuiRenderer::GuiRenderer(Window* window) :
	m_window(window)
{
}
projectSolar::GuiRenderer::~GuiRenderer()
{
}

void projectSolar::GuiRenderer::render()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode);
	
	for (auto const& [id, window] : m_guiWindows)
	{
		if (window->showFlag)
		{
			window->show();
		}
	}

	const ImGuiIO& io = ImGui::GetIO();

	ImGui::Render();
	int display_w;
	int display_h;
	glfwGetFramebufferSize((GLFWwindow*)m_window->getNativeWindow(), &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

bool projectSolar::GuiRenderer::addGuiWindow(const std::string& id, GuiWindow* window, bool show)
{
	if (m_guiWindows.contains(id))
	{
		return false;
	}

	m_guiWindows.try_emplace(id, window);
	m_guiWindows[id]->showFlag = show;
	
	return true;
}
bool projectSolar::GuiRenderer::showGuiWindow(const std::string& id, bool show)
{
	if (!m_guiWindows.contains(id))
	{
		return false;
	}
	
	m_guiWindows[id]->showFlag = show;

	return true;
}
bool projectSolar::GuiRenderer::delGuiWindow(const std::string& id)
{
	if (!m_guiWindows.contains(id))
	{
		return false;
	}

	m_guiWindows.erase(id);
	
	return true;
}
bool projectSolar::GuiRenderer::ifGuiWindowExists(const std::string& id) const
{
	return m_guiWindows.contains(id);
}
bool projectSolar::GuiRenderer::ifGuiWindowShown(const std::string& id) const
{
	if (!m_guiWindows.contains(id))
	{
		return false;
	}
	return m_guiWindows.at(id)->showFlag;
}
