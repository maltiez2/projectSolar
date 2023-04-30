#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include "Logger.h"

static uint32_t CompileShader(uint32_t type, const std::string source)
{
	uint32_t id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int32_t result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int32_t length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		char* message = (char*)alloca(length * sizeof(char)); // allocating memory in stack instead of heap
		glGetShaderInfoLog(id, length, &length, message);

		std::string shaderType = (type == GL_VERTEX_SHADER ? "vertex" : "fragment");
		LOG_ERROR((std::string)"Error on " + shaderType + " shader compilation: " + message);

		return 0;
	}

	return id;
}

static uint32_t CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	uint32_t program = glCreateProgram();
	uint32_t vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
	uint32_t fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

int main(void)
{
	LOG_INTT_CONSOLE("logs/log_graphicsTest.txt");
	
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (int error = glewInit(); error != GLEW_OK)
	{
		LOG_ERROR("Error on glew init: " + std::to_string(error));
		return error;
	}

	LOG_INFO((std::string)"GL version: " + (char*)glGetString(GL_VERSION));

	float positions[6] = {
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f
	};

	uint32_t buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	std::string vertexShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec4 position;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = position;\n"
		"}\n";
	std::string fragmentShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	color = vec4(1.0, 1.0, 0.0, 1.0);\n"
		"}\n";
	uint32_t shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteShader(shader);

	glfwTerminate();
	return 0;
}