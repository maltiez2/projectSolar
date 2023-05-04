#include "Window.h"
#include "Logger.h"

#include "imgui/imgui.h"
#include "opengl/imgui_impl_glfw.h"
#include "opengl/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

using namespace imGuiTest;

static void glfwErrorCallback(int error, const char* description)
{
	LOG_ERROR((std::string)"[OpenGL] (" + std::to_string(error) + "): " + description);
}

static void glfwResizeCallback(GLFWwindow* window, int width, int height)
{
	((Window*)glfwGetWindowUserPointer(window))->setSize(width, height);
}

Window::Window(const WindowProperties& properties)
{
	init(properties);
}
Window::~Window()
{
	shutdown();
}

uint32_t Window::getWidth() const
{
	return m_properties.width;
}
uint32_t Window::getHeight() const
{
	return m_properties.height;
}
void* Window::getNativeWindow()
{
	return m_window;
}
bool Window::isVSync() const
{
	return m_properties.VSync;
}

void Window::setVSync(bool enabled)
{
	glfwSwapInterval(enabled ? 1 : 0);
	m_properties.VSync = enabled;
}
void Window::setFont(const std::string& font)
{
	ImGui::GetIO().Fonts->AddFontFromFileTTF((fontsFolder + ("/" + font)).c_str(), 15.0f);
}
void Window::setSize(uint32_t width, uint32_t height)
{
	int currentWidth;
	int currentHeight;
	glfwGetWindowSize((GLFWwindow*)m_window, &currentWidth, &currentHeight);
	if (width != currentWidth || height != currentHeight)
	{
		glfwSetWindowSize((GLFWwindow*)m_window, width, height);
	}
	m_properties.width = width;
	m_properties.height = height;
	LOG_DEBUG("Set size");
}

void Window::init(const WindowProperties& properties)
{
	LOG_DEBUG("Window initialization started");
	
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit())
	{
		LOG_ERROR("Unable to initialize GLFW");
		return;
	}

	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primary);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinorVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	m_window = glfwCreateWindow(mode->width, mode->height, properties.title.c_str(), primary, nullptr);
	if (m_window == nullptr)
	{
		LOG_ERROR("Unable to create window");
		shutdown();
		return;
	}

	glfwSetWindowUserPointer((GLFWwindow*)m_window, this);

	m_properties = properties;

	glfwMakeContextCurrent((GLFWwindow*)m_window);
	setVSync(properties.VSync);

	glfwSetWindowSizeCallback((GLFWwindow*)m_window, glfwResizeCallback);

	setupImGui(properties.guiProperties);

	LOG_DEBUG("Window initialization complete");
}
void Window::shutdown()
{
	LOG_DEBUG("Window shutdown started");
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	
	glfwDestroyWindow((GLFWwindow*)m_window);
	glfwTerminate();

	LOG_DEBUG("Window shutdown complete");
}

void Window::setupImGui(const GuiProperties& properties)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	io.IniFilename = NULL;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 5.0f;
        style.ChildRounding = 5.0f;
        style.FrameRounding = 5.0f;
        style.GrabRounding = 5.0f;
        style.PopupRounding = 5.0f;
        style.ScrollbarRounding = 5.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

	setFont(properties.font);
}
