#pragma once

#include "Common.h"

#include <string>

class Texture
{
public:
	Texture(const std::string& path);
	~Texture();

	void bind(uint32_t slot = 0) const;
	void unbind() const;

	inline size_t getWidth() { return m_width; };
	inline size_t getHeight() { return m_height; };

private:
	uint32_t m_rendererID = 0;
	std::string m_filePath;
	uint8_t* m_localBuffer = nullptr;
	int m_width = 0;
	int m_height = 0;
	int m_BPP = 0;
};