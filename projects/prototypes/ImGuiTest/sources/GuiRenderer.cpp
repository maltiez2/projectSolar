#include "GuiRenderer.h"

#include "imgui/imgui.h"
#include "opengl/imgui_impl_glfw.h"
#include "opengl/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

imGuiTest::GuiRenderer::GuiRenderer(Window* window) :
	m_window(window)
{
}
imGuiTest::GuiRenderer::~GuiRenderer()
{
}

void imGuiTest::GuiRenderer::render()
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

	auto clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
	const ImGuiIO& io = ImGui::GetIO();

	ImGui::Render();
	int display_w;
	int display_h;
	glfwGetFramebufferSize((GLFWwindow*)m_window->getNativeWindow(), &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

bool imGuiTest::GuiRenderer::addGuiWindow(const std::string& id, GuiWindow* window, bool show)
{
	if (m_guiWindows.contains(id))
	{
		return false;
	}

	m_guiWindows.try_emplace(id, window);
	m_guiWindows[id]->showFlag = show;
	
	return true;
}
bool imGuiTest::GuiRenderer::showGuiWindow(const std::string& id, bool show)
{
	if (!m_guiWindows.contains(id))
	{
		return false;
	}
	
	m_guiWindows[id]->showFlag = show;

	return true;
}
bool imGuiTest::GuiRenderer::delGuiWindow(const std::string& id)
{
	if (!m_guiWindows.contains(id))
	{
		return false;
	}

	m_guiWindows.erase(id);
	
	return true;
}
bool imGuiTest::GuiRenderer::ifGuiWindowExists(const std::string& id) const
{
	return m_guiWindows.contains(id);
}
bool imGuiTest::GuiRenderer::ifGuiWindowShown(const std::string& id) const
{
	if (!m_guiWindows.contains(id))
	{
		return false;
	}
	return m_guiWindows.at(id)->showFlag;
}
