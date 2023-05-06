#pragma once

#include <stdint.h>

class IndexBuffer
{
public:
	IndexBuffer(const uint16_t* data, size_t count);
	~IndexBuffer();

	void bind() const;
	void unbind() const;

	inline size_t getCount() const { return m_count; };

private:
	uint32_t m_rendererID;
	size_t m_count;
};
