#pragma once

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"


class Renderer
{
public:
	Renderer();

	void clear() const;
	void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};
