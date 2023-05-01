#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <array>
#include <fstream>
#include <sstream>

#include "Logger.h"

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

int main(void)
{
	LOG_INTT_CONSOLE("logs/log_graphicsTest.txt");

	{
		GLFWwindow* window;

		/* Initialize the library */
		if (!glfwInit())
			return -1;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		/* Create a windowed mode window and its OpenGL context */
		window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
		if (!window)
		{
			glfwTerminate();
			return -1;
		}

		/* Make the window's context current */
		glfwMakeContextCurrent(window);

		glfwSwapInterval(1);

		if (int error = glewInit(); error != GLEW_OK)
		{
			LOG_ERROR("Error on glew init: " + std::to_string(error));
			return error;
		}

		LOG_INFO((std::string)"GL version: " + (char*)glGetString(GL_VERSION));

		float positions[] = {
			-0.5f,  -0.5f, // 0
			 0.5f,  -0.5f, // 1
			 0.5f,   0.5f, // 2
			-0.5f,   0.5f  // 3
		};

		uint16_t indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		VertexArray va;
		VertexBuffer vb(positions, 4 * 2 * sizeof(float));

		VertexBufferLayout layout;
		layout.push<float>(2);
		va.addBufer(vb, layout);

		IndexBuffer ib(indices, 6);

		Shader shader("resources/test_1_basic.shader");
		shader.bind();
		shader.setUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

		va.unbind();
		shader.unbind();
		vb.unbind();
		ib.unbind();

		Renderer renderer;

		float red = 0.0f;
		float redIncrement = 0.01f;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			renderer.clear();

			shader.bind();
			shader.setUniform4f("u_Color", red, 0.3f, 0.8f, 1.0f);

			renderer.draw(va, ib, shader);

			if ((red > 1.0f) || (red < 0.0f))
			{
				redIncrement = -redIncrement;
			}

			red += redIncrement;

			/* Swap front and back buffers */
			GLCall(glfwSwapBuffers(window));

			/* Poll for and process events */
			GLCall(glfwPollEvents());
		}
	}

	glfwTerminate();
	return 0;
}