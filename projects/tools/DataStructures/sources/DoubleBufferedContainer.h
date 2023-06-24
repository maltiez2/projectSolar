#pragma once

#include <set>
#include <vector>

namespace projectSolar::DataStructures
{
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
		std::vector<DataType> m_evenData = {};
		std::vector<DataType> m_oddData = {};
		std::set<size_t> m_invalidElemetns = {};
		bool m_parity = false;
		size_t m_size = 0;
	};

	template<typename DataType>
	class DoubleBuffVectorWithVersions
	{
	public:
		const uint64_t defautVersion = 1;
		const uint64_t invalidVersion = 0;
		
		struct Element
		{
			uint64_t version;
			DataType value;
		};
		
		std::vector<Element>& getData()
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
		std::vector<Element>& getBuffer()
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
				m_evenData.push_back({ defautVersion, value });
				m_oddData.push_back({ defautVersion, value });
			}
			else
			{
				index = m_invalidElemetns.extract(0).value();
				m_evenData[index].version += 1;
				m_evenData[index].value = value;
				m_oddData[index].version += 1;
				m_oddData[index].value = value;
			}
			m_size++;
			return index;
		}
		size_t addElementVersionEnforced(const Element& element)
		{
			size_t index;
			if (m_invalidElemetns.empty())
			{
				index = m_evenData.size();
				m_evenData.push_back({ element.version, element.value });
				m_oddData.push_back({ element.version, element.value });
			}
			else
			{
				index = m_invalidElemetns.extract(0).value();
				m_evenData[index].version = element.version;
				m_evenData[index].value = element.value;
				m_oddData[index].version = element.version;
				m_oddData[index].value = element.value;
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
		uint64_t getDataVersion(size_t index)
		{
			if (index >= m_size)
			{
				return invalidVersion;
			}

			if (m_parity)
			{
				return m_oddData[index].version;
			}
			else
			{
				return m_evenData[index].version;
			}
		}
		uint64_t getBufferVersion(size_t index)
		{
			if (index >= m_size)
			{
				return invalidVersion;
			}

			if (!m_parity)
			{
				return m_oddData[index].version;
			}
			else
			{
				return m_evenData[index].version;
			}
		}

	private:
		std::vector<Element> m_evenData = {};
		std::vector<Element> m_oddData = {};
		std::set<size_t> m_invalidElemetns = {};
		bool m_parity = false;
		size_t m_size = 0;
	};
}
