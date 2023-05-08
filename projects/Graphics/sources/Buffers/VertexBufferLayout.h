#pragma once

#include <GL/glew.h>

#include "Logger.h"


namespace projectSolar::Graphics
{
	struct VertexBufferElement
	{
		uint32_t type;
		uint32_t count;
		uint8_t normalized;

		static uint32_t getSizeOfType(uint32_t type)
		{
			switch (type)
			{
			case GL_DOUBLE:
				return 8;
			case GL_FLOAT:
			case GL_INT:
			case GL_UNSIGNED_INT:
				return 4;
			case GL_SHORT:
			case GL_UNSIGNED_SHORT:
				return 2;
			case GL_UNSIGNED_BYTE:
				return 1;
			default:
				__debugbreak();
				return 0;
			}
		}
	};

	class VertexBufferLayout
	{
	public:
		VertexBufferLayout() = default;

		template<typename T>
		void push(uint32_t count)
		{
			LOG_ASSERT(false, "Specialize it for a specific type")
		}

		template<>
		void push<float>(uint32_t count)
		{
			m_elements.emplace_back(GL_FLOAT, count, GL_FALSE);
			m_stride += VertexBufferElement::getSizeOfType(GL_FLOAT) * count;
		}

		template<>
		void push<double>(uint32_t count)
		{
			m_elements.emplace_back(GL_DOUBLE, count, GL_FALSE);
			m_stride += VertexBufferElement::getSizeOfType(GL_DOUBLE) * count;
		}

		template<>
		void push<uint32_t>(uint32_t count)
		{
			m_elements.emplace_back(GL_UNSIGNED_INT, count, GL_FALSE);
			m_stride += VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT) * count;
		}

		template<>
		void push<int32_t>(uint32_t count)
		{
			m_elements.emplace_back(GL_INT, count, GL_FALSE);
			m_stride += VertexBufferElement::getSizeOfType(GL_INT) * count;
		}

		template<>
		void push<uint16_t>(uint32_t count)
		{
			m_elements.emplace_back(GL_UNSIGNED_SHORT, count, GL_FALSE);
			m_stride += VertexBufferElement::getSizeOfType(GL_UNSIGNED_SHORT) * count;
		}

		template<>
		void push<int16_t>(uint32_t count)
		{
			m_elements.emplace_back(GL_SHORT, count, GL_FALSE);
			m_stride += VertexBufferElement::getSizeOfType(GL_SHORT) * count;
		}

		template<>
		void push<uint8_t>(uint32_t count)
		{
			m_elements.emplace_back(GL_UNSIGNED_BYTE, count, GL_TRUE);
			m_stride += VertexBufferElement::getSizeOfType(GL_UNSIGNED_BYTE) * count;
		}

		inline std::vector<VertexBufferElement> getElements() const { return m_elements; }
		inline uint32_t getStride() const { return m_stride; }

	private:
		std::vector<VertexBufferElement> m_elements;
		uint32_t m_stride;
	};
}
