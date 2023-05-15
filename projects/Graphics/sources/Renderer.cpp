#include "pch.h"

#include "Renderer.h"
#include "Buffers/IndexBuffer.h"
#include "Buffers/VertexArray.h"
#include "Shader.h"


using namespace projectSolar::Graphics;

Renderer::Renderer()
{
	glEnable(GL_PROGRAM_POINT_SIZE);
}

void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, uint32_t count) const
{
	shader.bind();
	va.bind();
	ib.bind();

	glDrawElements(GL_POINTS, count, GL_UNSIGNED_INT, nullptr);
}
