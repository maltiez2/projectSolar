#include "pch.h"

#include "Window.h"


using namespace projectSolar;

static void glfwErrorCallback(int error, const char* description)
{
	LOG_ERROR("[OpenGL] (", error, "): ", description);
}

Window::Window(const WindowProperties& properties) :
	m_eventsManager(this),
	m_inputManager(this)
{
	init(properties);
}
Window::~Window()
{
	shutdown();
}

void Window::startFrame()
{
	glfwPollEvents();
	glfwSwapBuffers(m_window);
	glClear(GL_COLOR_BUFFER_BIT);
}

uint32_t Window::getWidth() const
{
	return m_properties.width;
}
uint32_t Window::getHeight() const
{
	return m_properties.height;
}
GLFWwindow* Window::getNativeWindow()
{
	return m_window;
}
bool Window::isVSync() const
{
	return m_properties.VSync;
}
InputEventsManager& Window::getEventsManager()
{
	return m_eventsManager;
}
InputManager& Window::getInputManager()
{
	return m_inputManager;
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
	glfwGetWindowSize(m_window, &currentWidth, &currentHeight);
	if (width != currentWidth || height != currentHeight)
	{
		glfwSetWindowSize(m_window, width, height);
	}
	m_properties.width = width;
	m_properties.height = height;
	LOG_DEBUG("Set size");
}

void Window::init(const WindowProperties& properties)
{
	LOG_DEBUG("Window initialization started");
	
	m_properties = properties;
	m_properties.eventsManager = &m_eventsManager;

	glfwSetErrorCallback(glfwErrorCallback);
	int success = glfwInit();
	LOG_ASSERT(success, "Failed to initialize GLFW")

	GLFWmonitor* monitor = setUpFullscreen();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinorVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
	
	m_window = glfwCreateWindow(properties.width, properties.height, properties.title.c_str(), monitor, nullptr);
	LOG_ASSERT(m_window != nullptr, "Failed to create window")

	success = glewInit();
	LOG_ASSERT(success, "Failed to initialize GLEW")

	glfwSetWindowUserPointer(m_window, &m_properties);
	glfwMakeContextCurrent(m_window);
	setVSync(properties.VSync);

	int error = glewInit();
	LOG_ASSERT(error == GLEW_OK, "Error on glew init: ", error)
	LOG_INFO("GL version: ", (char*)glGetString(GL_VERSION));

	m_eventsManager.setupCallbacks(); // need to be before setupImGui
	setupImGui(properties.guiProperties);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	LOG_DEBUG("Window initialization complete");
}
void Window::shutdown()
{
	LOG_DEBUG("Window shutdown started");
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	
	glfwDestroyWindow(m_window);
	glfwTerminate();

	LOG_DEBUG("Window shutdown complete");
}

void Window::setupImGui(const GuiProperties& properties)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.IniFilename = nullptr;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

	setFont(properties.font);
}
GLFWmonitor* Window::setUpFullscreen()
{
	if (!m_properties.fullScreen)
	{
		return nullptr;
	}
	
	int count;
	GLFWmonitor* monitor;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	if (m_properties.monitor < 0 || m_properties.monitor >= count)
	{
		monitor = glfwGetPrimaryMonitor();
	}
	else
	{
		monitor = monitors[m_properties.monitor];
	}

	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	
	m_properties.width = mode->width;
	m_properties.height = mode->height;

	return monitor;
}
