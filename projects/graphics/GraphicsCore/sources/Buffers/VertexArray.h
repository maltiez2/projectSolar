#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"


namespace projectSolar
{
	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void addBufer(const VertexBuffer& vb, const VertexBufferLayout& layout);

		void bind() const;
		void unbind() const;

	private:
		uint32_t m_rendererID;
	};
}
