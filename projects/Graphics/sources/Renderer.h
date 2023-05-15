#pragma once


namespace projectSolar::Graphics
{
	class VertexArray;
	class IndexBuffer;
	class Shader;
	
	class Renderer
	{
	public:
		Renderer();

		void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, uint32_t count) const;
	};
}
