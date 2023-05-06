#pragma once

#include <stdint.h>

namespace projectSolar
{
	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, size_t size);
		~VertexBuffer();

		void updateData(const void* data, size_t size) const;
		void bind() const;
		void unbind() const;

	private:
		uint32_t m_rendererID;
	};
}
