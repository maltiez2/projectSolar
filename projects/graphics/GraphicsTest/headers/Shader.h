#pragma once

#include <string>
#include <unordered_map>

class Shader
{
public:
	Shader(const std::string& filename);
	~Shader();

	void bind() const;
	void unbind() const;

	// Set uniforms
	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

private:
	struct ShaderProgramSource
	{
		std::string vertexSource;
		std::string fragmentSource;
	};
	
	uint32_t m_rendererID = 0;
	std::string m_filename;
	std::unordered_map<std::string, int64_t> m_uniformLocationCache;

	int64_t getUniformLocation(const std::string& name);

	ShaderProgramSource parseShader();
	uint32_t compileShader(uint32_t type, const std::string source);
	uint32_t createShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
};
