#pragma once

#include <stdint.h>

class VertexBuffer
{
public:
	VertexBuffer(const void* data, size_t size);
	~VertexBuffer();

	void bind() const;
	void unbind() const;

private:
	uint32_t m_rendererID;
};
