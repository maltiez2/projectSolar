#include "GraphicsCore.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Application.h"

#include "GUIRenderer.h"
#include "CentralRenderer.h"
#include "Logger.h"


using namespace projectSolar;

void Application::run()
{
    LOG_INTT_CONSOLE("logs/log_imGuiTest.txt");

    Window window;

    // *** Central ***

    float positions[] = {
            100.0f, 100.0f, 0.0f, 0.0f, // 0
            300.0f, 100.0f, 1.0f, 0.0f, // 1
            300.0f, 300.0f, 1.0f, 1.0f, // 2
            100.0f, 300.0f, 0.0f, 1.0f  // 3
    };

    uint16_t indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    VertexArray va;
    VertexBuffer vb(positions, 4 * 4 * sizeof(float));

    VertexBufferLayout layout;
    layout.push<float>(2);
    layout.push<float>(2);
    va.addBufer(vb, layout);

    IndexBuffer ib(indices, 6);

    glm::mat4 proj = glm::ortho(0.0f, (float)window.getWidth(), 0.0f, (float)window.getHeight(), -1.0f, 1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 100, 0));

    glm::mat4 mvp = proj * view * model;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shader("resources/test_1_basic.shader");
    shader.bind();
    shader.setUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
    shader.setUniformMat4f("u_MVP", mvp);

    Texture texture("resources/dumpling.png");
    texture.bind();
    shader.setUniform1i("u_Texture", 0);

    va.unbind();
    shader.unbind();
    vb.unbind();
    ib.unbind();

    Renderer centralRenderer;

    // *** GUI ***
    auto guiRenderer = GuiRenderer(&window);

    auto guiWindow = NotificationWindow("Test window", "Test text of test window");
    auto debugWindow = DebugWindow();
    auto demoWindow = DemoWindow();

    guiRenderer.addGuiWindow("test", &guiWindow, true);
    guiRenderer.addGuiWindow("debug", &debugWindow, true);
    guiRenderer.addGuiWindow("demo", &demoWindow, true);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose((GLFWwindow*)window.getNativeWindow()))
    {
        glfwPollEvents();

        // *** Central ***

        centralRenderer.clear();
        shader.bind();
        centralRenderer.draw(va, ib, shader);

        // *** GUI ***

        demoWindow.showFlag = debugWindow.showDemoWindow;
        guiRenderer.render();

        

        glfwSwapBuffers((GLFWwindow*)window.getNativeWindow());
    }
}

