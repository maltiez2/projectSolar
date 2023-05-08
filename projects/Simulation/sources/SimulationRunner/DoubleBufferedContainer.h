#pragma once

#include <set>
#include <vector>
#include <array>

namespace projectSolar::Simulation
{
	/*template<typename DataType>
	class TripleBuffVector
	{
	public:
		std::vector<DataType>& getData(uint8_t index = 0)
		{
			switch (m_state[index])
			{
			case 0:
				return m_vector_0;
			case 1:
				return m_vector_1;
			case 2:
				return m_vector_2;
			default:
				LOG_ASSERT(false, "[TripleBuffVector] Wrong vector index: ", index)
			}

			return m_vector_0;
		}
		std::vector<DataType>& getBuffer()
		{
			return getData(1);
		}
		std::vector<DataType>& getSecondBuffer()
		{
			return getData(2);
		}

		void swap()
		{
			swapBuffer = m_state[0];
			m_state[0] = m_state[1];
			m_state[1] = swapBuffer
		}
		void swapToSecond()
		{
			swapBuffer = m_state[0];
			m_state[0] = m_state[2];
			m_state[2] = swapBuffer
		}
		void reserve(size_t size)
		{
			m_vector_0.reserve(size);
			m_vector_1.reserve(size);
			m_vector_2.reserve(size);
		}
		void clear()
		{
			m_vector_0.clear();
			m_vector_1.clear();
			m_vector_2.clear();
			m_invalidElemetns.clear();
			m_size = 0;
			m_parity = false;
		}

		size_t addElement(const DataType& value)
		{
			size_t index;
			if (m_invalidElemetns.empty())
			{
				index = m_evenData.size();
				m_vector_0.push_back(value);
				m_vector_1.push_back(value);
				m_vector_2.push_back(value);
			}
			else
			{
				index = m_invalidElemetns.extract(0).value();
				m_vector_0[index] = value;
				m_vector_1[index] = value;
				m_vector_2[index] = value;
			}
			m_size++;
			return index;
		}
		bool delElement(size_t index)
		{
			if (index >= m_size)
			{
				return false;
			}

			if (m_invalidElemetns.contains(index))
			{
				return false;
			}

			m_invalidElemetns.insert(index);
			m_size--;

			return true;
		}
		bool isValid(size_t index) const
		{
			if (index >= m_size)
			{
				return false;
			}

			return !m_invalidElemetns.contains(index);
		}

	private:
		uint8_t swapBuffer;
		std::array<uint8_t, 3> m_state = { 0, 1, 2 };
		std::vector<DataType> m_vector_0;
		std::vector<DataType> m_vector_1;
		std::vector<DataType> m_vector_2;

		std::set<size_t> m_invalidElemetns;
		size_t m_size = 0;
	};*/
	
	template<typename DataType>
	class DoubleBuffVector
	{
	public:
		std::vector<DataType>& getData()
		{
			if (m_parity)
			{
				return m_oddData;
			}
			else
			{
				return m_evenData;
			}
		}
		std::vector<DataType>& getBuffer()
		{
			if (m_parity)
			{
				return m_evenData;
			}
			else
			{
				return m_oddData;
			}
		}

		void swap()
		{
			m_parity = !m_parity;
		}
		void reserve(size_t size)
		{
			m_evenData.reserve(size);
			m_oddData.reserve(size);
		}
		void clear()
		{
			m_evenData.clear();
			m_oddData.clear();
			m_invalidElemetns.clear();
			m_size = 0;
			m_parity = false;
		}
		constexpr size_t size() const
		{
			return m_size;
		}

		size_t addElement(const DataType& value)
		{
			size_t index;
			if (m_invalidElemetns.empty())
			{
				index = m_evenData.size();
				m_evenData.push_back(value);
				m_oddData.push_back(value);
			}
			else
			{
				index = m_invalidElemetns.extract(0).value();
				m_evenData[index] = value;
				m_oddData[index] = value;
			}
			m_size++;
			return index;
		}
		bool delElement(size_t index)
		{
			if (index >= m_size)
			{
				return false;
			}
			
			if (m_invalidElemetns.contains(index))
			{
				return false;
			}

			m_invalidElemetns.insert(index);
			m_size--;

			return true;
		}
		bool isValid(size_t index) const
		{
			if (index >= m_size)
			{
				return false;
			}

			return !m_invalidElemetns.contains(index);
		}

	private:
		std::vector<DataType> m_evenData;
		std::vector<DataType> m_oddData;
		std::set<size_t> m_invalidElemetns;
		bool m_parity = false;
		size_t m_size = 0;
	};
}
