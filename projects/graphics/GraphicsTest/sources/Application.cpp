#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <array>
#include <fstream>
#include <sstream>

#include "Logger.h"

#define ASSERT(x) if (!(x)) __debugbreak()
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, uint32_t line)
{
	bool errorOccurred = false;
	while (GLenum error = glGetError())
	{
		LOG_ERROR((std::string)"[OpenGL] (" + std::to_string(error) + "): " + function + " " + file + ": " + std::to_string(line));
		errorOccurred = true;
	}
	return !errorOccurred;
}

struct ShaderProgramSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filePath)
{
	std::ifstream stream(filePath);

	enum class ShaderType
	{
		NONE = 1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	std::string line;
	std::array<std::stringstream, 2> shaderSourceCodeBuffers;
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			shaderSourceCodeBuffers[(int)type] << line << '\n';
		}
	}

	return { shaderSourceCodeBuffers[0].str(), shaderSourceCodeBuffers[1].str() };
}

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

	uint32_t indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	uint32_t vertexArrayObject;
	GLCall(glGenVertexArrays(1, &vertexArrayObject));
	GLCall(glBindVertexArray(vertexArrayObject));

	uint32_t buffer;
	GLCall(glGenBuffers(1, &buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

	uint32_t indexBufferObject;
	GLCall(glGenBuffers(1, &indexBufferObject));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW));

	ShaderProgramSource source = ParseShader("resources/test_1_basic.shader");

	uint32_t shader = CreateShader(source.vertexSource, source.fragmentSource);
	GLCall(glUseProgram(shader));

	uint32_t location = glGetUniformLocation(shader, "u_Color");
	ASSERT(location != -1);
	GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

	GLCall(glBindVertexArray(0));
	GLCall(glUseProgram(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));


	float red = 0.0f;
	float redIncrement = 0.01f;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		GLCall(glUseProgram(shader));
		GLCall(glUniform4f(location, red, 0.3f, 0.8f, 1.0f));

		GLCall(glBindVertexArray(vertexArrayObject));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject));

		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

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

	glDeleteShader(shader);

	glfwTerminate();
	return 0;
}