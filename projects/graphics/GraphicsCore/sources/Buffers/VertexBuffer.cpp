#include "../pch.h"

#include "VertexBuffer.h"


using namespace projectSolar;

VertexBuffer::VertexBuffer(const void* data, size_t size)
{
	glGenBuffers(1, &m_rendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_rendererID);
}

void VertexBuffer::updateData(const void* data, size_t size) const
{
	bind();
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	unbind();
}

void VertexBuffer::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void VertexBuffer::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
