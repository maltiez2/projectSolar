#include <string>

#include "Common.h"
#include "Logger.h"

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, uint32_t line)
{
	bool errorOccurred = false;
	while (GLenum error = glGetError())
	{
		LOG_ERROR((std::string)"[OpenGL] (" + std::to_string(error) + "): " + function + " " + file + ": " + std::to_string(line));
		errorOccurred = true;
	}
	return !errorOccurred;
}