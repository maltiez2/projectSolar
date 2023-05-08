#include "pch.h"

#include "GuiRenderer.h"
#include "GuiWindow.h"


using namespace projectSolar::Graphics;

GuiRenderer::GuiRenderer(Window& window, GuiWindowsManager& windowsManager) :
	m_window(window),
	m_windowsManager(windowsManager)
{
}
void GuiRenderer::render()
{
	begin();

	for (auto const& [id, window] : m_windowsManager.getWindows())
	{
		if (window->showFlag)
		{
			window->show();
		}
	}

	end();
}
void GuiRenderer::begin()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode);
}
void GuiRenderer::end()
{
	const ImGuiIO& io = ImGui::GetIO();

	ImGui::Render();
	int display_w;
	int display_h;
	glfwGetFramebufferSize((GLFWwindow*)m_window.getNativeWindow(), &display_w, &display_h);
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


GuiWindowsManager::~GuiWindowsManager()
{
	for (auto& [id, pointer] : m_windows)
	{
		delete(pointer);
	}
}
bool GuiWindowsManager::del(const std::string& id)
{
	if (!m_windows.contains(id))
	{
		LOG_ERROR("[GuiWindowsManager][del()] Window with id '" + id + "' does not exist");
		return false;
	}

	delete(m_windows[id]);
	m_windows.erase(id);

	return true;
}
bool GuiWindowsManager::show(const std::string& id, bool show)
{
	if (!m_windows.contains(id))
	{
		LOG_ERROR("[GuiWindowsManager][show()] Window with id '" + id + "' does not exist");
		return false;
	}

	m_windows[id]->showFlag = show;

	return true;
}
bool GuiWindowsManager::exist(const std::string& id)
{
	return m_windows.contains(id);
}
bool GuiWindowsManager::shown(const std::string& id)
{
	if (!m_windows.contains(id))
	{
		LOG_ERROR("[GuiWindowsManager][shown()] Window with id '" + id + "' does not exist");
		return false;
	}

	return m_windows[id]->showFlag;
}
const std::unordered_map<std::string, GuiWindow*>& GuiWindowsManager::getWindows()
{
	return m_windows;
}
