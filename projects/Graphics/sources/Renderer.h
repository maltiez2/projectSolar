#pragma once

#include "Buffers/IndexBuffer.h"
#include "Buffers/VertexArray.h"
#include "Shader.h"


namespace projectSolar
{
	class Renderer
	{
	public:
		Renderer();

		void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	};
}
