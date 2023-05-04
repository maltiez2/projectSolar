#pragma once

#include <vector>

struct VertexBufferElement
{
	uint32_t type;
	size_t count;
	uint8_t normalized;

	static size_t getSizeOfType(uint32_t type)
	{
		switch (type)
		{
		case GL_FLOAT:
			return 4;
		case GL_UNSIGNED_INT:
			return 4;
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
	void push(size_t count)
	{
		__debugbreak();
	}

	template<>
	void push<float>(size_t count)
	{
		m_elements.emplace_back(GL_FLOAT, count, GL_FALSE);
		m_stride += VertexBufferElement::getSizeOfType(GL_FLOAT) * count;
	}

	template<>
	void push<uint32_t>(size_t count)
	{
		m_elements.emplace_back(GL_UNSIGNED_INT, count, GL_FALSE);
		m_stride += VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT) * count;
	}

	template<>
	void push<uint16_t>(size_t count)
	{
		m_elements.emplace_back(GL_UNSIGNED_SHORT, count, GL_FALSE);
		m_stride += VertexBufferElement::getSizeOfType(GL_UNSIGNED_SHORT) * count;
	}

	template<>
	void push<uint8_t>(size_t count)
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
