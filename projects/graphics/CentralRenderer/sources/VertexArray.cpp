#include "GraphicsCore.h"

#include "VertexArray.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_rendererID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_rendererID);
}

void VertexArray::addBufer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	bind();
	vb.bind();
	const auto& elements = layout.getElements();
	uint32_t offset = 0;
	for (size_t i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), (const void*)offset);
		offset += element.count * VertexBufferElement::getSizeOfType(element.type);
	}
}

void VertexArray::bind() const
{
	glBindVertexArray(m_rendererID);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}
