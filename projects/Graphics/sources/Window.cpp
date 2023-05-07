#include "pch.h"

#include "Window.h"


using namespace projectSolar;

static void glfwErrorCallback(int error, const char* description)
{
	LOG_ERROR("[OpenGL] (", error, "): ", description);
}

Window::Window(const WindowProperties& properties)
{
	init(properties);
}
Window::~Window()
{
	shutdown();
}

void projectSolar::Window::startFrame()
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
void projectSolar::Window::setEventCallback(const Window::eventCallbackFunction& callback)
{
	m_properties.eventCallback = callback;
}

void Window::init(const WindowProperties& properties)
{
	LOG_DEBUG("Window initialization started");
	
	glfwSetErrorCallback(glfwErrorCallback);
	int success = glfwInit();
	LOG_ASSERT(success, "Failed to initialize GLFW")

	GLFWmonitor* monitor = setUpFullscreen();

	m_properties.fullScreen = properties.fullScreen;
	m_properties.title = properties.title;
	m_properties.VSync = properties.VSync;
	m_properties.height = properties.height;
	m_properties.width = properties.width;
	m_properties.monitor = properties.monitor;
	
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

	if (int error = glewInit(); error != GLEW_OK)
	{
		LOG_ERROR("Error on glew init: ", error);
		return;
	}

	LOG_INFO("GL version: ", (char*)glGetString(GL_VERSION));

	setupEvents(); // need to be before setupImGui
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
void projectSolar::Window::setupEvents()
{
	
	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);
			data.eventCallback(event);
		});

	glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.eventCallback(event);
		});

	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.eventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.eventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, true);
				data.eventCallback(event);
				break;
			}
			}
		});

	glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.eventCallback(event);
		});

	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.eventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.eventCallback(event);
				break;
			}
			}
		});

	glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.eventCallback(event);
		});

	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.eventCallback(event);
		});
}
GLFWmonitor* projectSolar::Window::setUpFullscreen()
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
