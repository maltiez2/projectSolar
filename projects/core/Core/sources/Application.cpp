#include "Application.h"

#include "GUIRenderer.h"
#include "Logger.h"

#include <GLFW/glfw3.h>

using namespace projectSolar;

void Application::run()
{
    LOG_INTT_CONSOLE("logs/log_imGuiTest.txt");

    auto window = Window();

    auto renderer = GuiRenderer(&window);

    auto guiWindow = NotificationWindow("Test window", "Test text of test window");
    auto debugWindow = DebugWindow();
    auto demoWindow = DemoWindow();

    renderer.addGuiWindow("test", &guiWindow, true);
    renderer.addGuiWindow("debug", &debugWindow, true);
    renderer.addGuiWindow("demo", &demoWindow, true);

    while (!glfwWindowShouldClose((GLFWwindow*)window.getNativeWindow()))
    {
        glfwPollEvents();

        demoWindow.showFlag = debugWindow.showDemoWindow;

        renderer.render();

        glfwSwapBuffers((GLFWwindow*)window.getNativeWindow());
    }
}

