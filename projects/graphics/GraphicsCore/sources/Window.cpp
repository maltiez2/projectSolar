#include "pch.h"

#include "Window.h"


using namespace projectSolar;

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

bool projectSolar::Window::startFrame()
{
	glfwSwapBuffers(m_window);
	return !glfwWindowShouldClose(m_window);
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
	
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit())
	{
		LOG_ERROR("Unable to initialize GLFW");
		return;
	}

	//GLFWmonitor* primary = glfwGetPrimaryMonitor(); // for release
	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count); // for debug
	int monitorNumber = 2;
	const GLFWvidmode* mode = glfwGetVideoMode(monitors[monitorNumber]);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinorVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
	
	m_window = glfwCreateWindow(mode->width, mode->height, properties.title.c_str(), monitors[monitorNumber], nullptr);
	if (m_window == nullptr)
	{
		LOG_ERROR("Unable to create window");
		shutdown();
		return;
	}

	glewInit();

	glfwSetWindowUserPointer(m_window, this);

	m_properties = properties;

	glfwMakeContextCurrent(m_window);
	setVSync(properties.VSync);

	glfwSetWindowSizeCallback(m_window, glfwResizeCallback);

	if (int error = glewInit(); error != GLEW_OK)
	{
		LOG_ERROR("Error on glew init: " + std::to_string(error));
		return;
	}

	LOG_INFO((std::string)"GL version: " + (char*)glGetString(GL_VERSION));

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
