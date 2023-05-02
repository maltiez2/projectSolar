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
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

		int windowWidth = 640;
		int windowHeight = 480;

		/* Create a windowed mode window and its OpenGL context */
		window = glfwCreateWindow(windowWidth, windowHeight, "Hello World", nullptr, nullptr);
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
			100.0f, 100.0f, 0.0f, 0.0f, // 0
			300.0f, 100.0f, 1.0f, 0.0f, // 1
			300.0f, 300.0f, 1.0f, 1.0f, // 2
			100.0f, 300.0f, 0.0f, 1.0f  // 3
		};

		uint16_t indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		VertexArray va;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));

		VertexBufferLayout layout;
		layout.push<float>(2);
		layout.push<float>(2);
		va.addBufer(vb, layout);

		IndexBuffer ib(indices, 6);

		glm::mat4 proj  = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1.0f);
		glm::mat4 view  = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 100, 0));

		glm::mat4 mvp = proj * view * model;

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

		Renderer renderer;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			renderer.clear();

			shader.bind();

			renderer.draw(va, ib, shader);

			/* Swap front and back buffers */
			GLCall(glfwSwapBuffers(window));

			/* Poll for and process events */
			GLCall(glfwPollEvents());
		}
	}

	glfwTerminate();
	return 0;
}