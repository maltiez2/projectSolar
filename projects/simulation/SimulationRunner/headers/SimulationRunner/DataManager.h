#pragma once

#include "SimulationCommon.h"
#include "DoubleBufferedContainer.h"

#include <stdio.h>

namespace projectSolar::Simulation
{
	class DataManager
	{
	public:
		DoubleBuffVector<Components::Attractor::Data>  attractorsData;
		DoubleBuffVector<Components::Attractant::Data> attractantsData;
		DoubleBuffVector<Components::Propulsed::Data>  propulsedData;

		void swap();

		errno_t save(std::string_view filePath);
		errno_t load(std::string_view filePath);

	private:
		class Serializer
		{
		public:
			enum class fileMode
			{
				readBytes,
				writeBytes
			};

			template<fileMode mode>
			errno_t open(std::string_view filePath)
			{
				switch (mode)
				{
				case fileMode::readBytes:
					return fopen_s(&p_file, filePath.data(), "rb");
				case fileMode::writeBytes:
					return fopen_s(&p_file, filePath.data(), "wb");
				default:
					return -1;
				}
			}

			template<fileMode mode = fileMode::readBytes>
			void close()
			{
				std::fclose(p_file);
			}

			template<typename DataType>
			void serialize(DoubleBuffVector<DataType>& input)
			{
				const auto& data = input.getData();

				size_t dataSize = size<DataType>();
				size_t dataAmount = data.size();
				std::fwrite(&dataSize, sizeof(dataSize), 1, p_file);
				std::fwrite(&dataAmount, sizeof(dataAmount), 1, p_file);
				m_buffer.resize(dataSize);
				for (auto index = 0; index < data.size(); index++)
				{
					write(data[index]);
				}
				m_buffer.clear();
			}

			template<typename DataType>
			void deserialize(DoubleBuffVector<DataType>& input)
			{
				input.clear();
				
				size_t dataSize;
				size_t dataAmount;
				std::fread(&dataSize, sizeof(dataSize), 1, p_file);
				std::fread(&dataAmount, sizeof(dataAmount), 1, p_file);
				DataType buffer;
				input.reserve(dataAmount);
				m_buffer.resize(dataSize);
				for (auto index = 0; index < dataAmount; index++)
				{
					read(buffer);
					input.addElement(buffer);
				}
				m_buffer.clear();
			}

		private:
			std::FILE* p_file = nullptr;
			std::vector<double> m_buffer = {};

			template<typename DataType>
			size_t size() const
			{
				static_assert(true, "Specialize it for a specific type");
				return 0;
			}
			template<>
			size_t size<Components::Attractor::Data>() const
			{
				return 7;
			}
			template<>
			size_t size<Components::Attractant::Data>() const
			{
				return 7;
			}
			template<>
			size_t size<Components::Propulsed::Data>() const
			{
				return 10;
			}

			template<typename DataType>
			void write(const DataType& data)
			{
				static_assert(true, "Specialize it for a specific type");
			}
			template<>
			void write<Components::Attractor::Data>(const Components::Attractor::Data& data)
			{
				m_buffer[0] = data.mass;
				for (uint8_t index = 0; index < 3; index++)
				{
					m_buffer[index + 1] = data.position[index];
					m_buffer[index + 4] = data.velocity[index];
				}
				std::fwrite(m_buffer.data(), sizeof(double), 7, p_file);
			}
			template<>
			void write<Components::Attractant::Data>(const Components::Attractant::Data& data)
			{
				m_buffer[0] = data.mass;
				for (uint8_t index = 0; index < 3; index++)
				{
					m_buffer[index + 1] = data.position[index];
					m_buffer[index + 4] = data.velocity[index];
				}
				std::fwrite(m_buffer.data(), sizeof(double), 7, p_file);
			}
			template<>
			void write<Components::Propulsed::Data>(const Components::Propulsed::Data& data)
			{
				m_buffer[0] = data.mass;
				for (uint8_t index = 0; index < 3; index++)
				{
					m_buffer[index + 1] = data.position[index];
					m_buffer[index + 4] = data.velocity[index];
					m_buffer[index + 7] = data.propulsion[index];
				}
				std::fwrite(m_buffer.data(), sizeof(double), 10, p_file);
			}

			template<typename DataType>
			void read(DataType& data)
			{
				static_assert(true, "Specialize it for a specific type");
			}
			template<>
			void read<Components::Attractor::Data>(Components::Attractor::Data& data)
			{
				std::fread(m_buffer.data(), sizeof(double), 7, p_file);
				data.mass = m_buffer[0];
				data.position = { m_buffer[1], m_buffer[2], m_buffer[3] };
				data.velocity = { m_buffer[4], m_buffer[5], m_buffer[6] };
			}
			template<>
			void read<Components::Attractant::Data>(Components::Attractant::Data& data)
			{
				std::fread(m_buffer.data(), sizeof(double), 7, p_file);
				data.mass = m_buffer[0];
				data.position = { m_buffer[1], m_buffer[2], m_buffer[3] };
				data.velocity = { m_buffer[4], m_buffer[5], m_buffer[6] };
			}
			template<>
			void read<Components::Propulsed::Data>(Components::Propulsed::Data& data)
			{
				std::fread(m_buffer.data(), sizeof(double), 10, p_file);
				data.mass = m_buffer[0];
				data.position   = { m_buffer[1], m_buffer[2], m_buffer[3] };
				data.velocity   = { m_buffer[4], m_buffer[5], m_buffer[6] };
				data.propulsion = { m_buffer[7], m_buffer[8], m_buffer[9] };
			}
		};

	};
}