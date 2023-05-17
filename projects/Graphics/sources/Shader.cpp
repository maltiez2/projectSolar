#include "pch.h"

#include "Shader.h"


using namespace projectSolar::Graphics;

Shader::Shader(const std::string& filename) :
	m_filename(filename)
{
	ShaderProgramSource source = parseShader();
	m_rendererID = createShader(source.vertexSource, source.fragmentSource);
}

Shader::~Shader()
{
	glDeleteProgram(m_rendererID);
}

Shader::ShaderProgramSource Shader::parseShader()
{
	std::ifstream stream(m_filename);

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

uint32_t Shader::compileShader(uint32_t type, const std::string source)
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

uint32_t Shader::createShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	uint32_t program = glCreateProgram();
	uint32_t vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
	uint32_t fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

void Shader::bind() const
{
	glUseProgram(m_rendererID);
}

void Shader::unbind() const
{
	glUseProgram(0);
}

void Shader::setUniform1f(const std::string& name, float v0)
{
	glUniform1f(getUniformLocation(name), v0);
}
void Shader::setUniform2f(const std::string& name, float v0, float v1)
{
	glUniform2f(getUniformLocation(name), v0, v1);
}
void Shader::setUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(getUniformLocation(name), v0, v1, v2);
}
void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::setUniform1i(const std::string& name, int value)
{
	glUniform1i(getUniformLocation(name), value);
}

int32_t Shader::getUniformLocation(const std::string& name)
{
	if (m_uniformLocationCache.contains(name))
	{
		return m_uniformLocationCache[name];
	}
	
	int32_t location = glGetUniformLocation(m_rendererID, name.c_str());
	if (location == -1)
	{
		LOG_WARN("Uniform '" + name + "' does not exist");
	}
	else
	{
		m_uniformLocationCache[name] = location;
	}
	return location;
}
