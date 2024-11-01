#pragma once

#include <unordered_map>
#include <string>
#include <glm/glm.hpp>


namespace projectSolar::Graphics
{
	class Shader
	{
	public:
		Shader(const std::string& filename);
		~Shader();

		void bind() const;
		void unbind() const;

		// Set uniforms
		void setUniform1f(const std::string& name, float v0);
		void setUniform2f(const std::string& name, float v0, float v1);
		void setUniform3f(const std::string& name, float v0, float v1, float v2);
		void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void setUniform1i(const std::string& name, int value);
		void setUniformMat4f(const std::string& name, const glm::mat4& matrix);

	private:
		struct ShaderProgramSource
		{
			std::string vertexSource;
			std::string fragmentSource;
		};

		uint32_t m_rendererID = 0;
		std::string m_filename;
		std::unordered_map<std::string, int32_t> m_uniformLocationCache;

		int32_t getUniformLocation(const std::string& name);

		ShaderProgramSource parseShader();
		uint32_t compileShader(uint32_t type, const std::string source);
		uint32_t createShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	};
}
