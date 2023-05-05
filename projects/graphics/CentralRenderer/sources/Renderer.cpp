#include "GraphicsCore.h"

#include "Renderer.h"
#include "Logger.h"

Renderer::Renderer()
{
	glEnable(GL_PROGRAM_POINT_SIZE);
}

void Renderer::clear() const
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.bind();
	va.bind();
	ib.bind();

	glDrawElements(GL_POINTS, ib.getCount(), GL_UNSIGNED_INT, nullptr);
}
