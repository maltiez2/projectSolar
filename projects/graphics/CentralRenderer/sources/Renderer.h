#pragma once

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

namespace projectSolar
{
	class Renderer
	{
	public:
		Renderer();

		void clear() const;
		void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	};
}
