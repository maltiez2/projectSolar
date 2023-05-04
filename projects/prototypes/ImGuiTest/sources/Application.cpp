#include "Application.h"
#include "GuiRenderer.h"
#include "Window.h"
#include "Logger.h"

#include "imgui/imgui.h"
#include "opengl/imgui_impl_glfw.h"
#include "opengl/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

using namespace imGuiTest;

Application::Application()
{
}

Application::~Application()
{
}

void Application::run()
{
    LOG_INTT_CONSOLE("logs/log_imGuiTest.txt");

    auto window = imGuiTest::Window();
    
    auto renderer = imGuiTest::GuiRenderer(&window);

    auto guiWindow = imGuiTest::NotificationWindow("Test window", "Test text of test window");
    auto debugWindow = imGuiTest::DebugWindow();
    auto demoWindow = imGuiTest::DemoWindow();

    renderer.addGuiWindow("test", &guiWindow, true);
    renderer.addGuiWindow("debug", &debugWindow, true);
    renderer.addGuiWindow("demo", &demoWindow, true);

    // Main loop
    while (!glfwWindowShouldClose((GLFWwindow*)window.getNativeWindow()))
    {
        glfwPollEvents();

        demoWindow.showFlag = debugWindow.showDemoWindow;

        renderer.render();

        glfwSwapBuffers((GLFWwindow*)window.getNativeWindow());
    }
}
