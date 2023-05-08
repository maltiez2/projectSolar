#pragma once

#include "DoubleBufferedContainer.h"
#include "Logger.h"

#include <stdio.h>
#include <Eigen/Eigen>


namespace projectSolar::Simulation
{
	struct AttractorData
	{
		double mass;
		Eigen::Vector3d position;
		Eigen::Vector3d velocity;
	};

	struct AttractantData
	{
		Eigen::Vector3d position;
		Eigen::Vector3d velocity;
	};

	struct PropulsedData
	{
		Eigen::Vector3d position;
		Eigen::Vector3d velocity;
		Eigen::Vector3d propulsion;
	};


	class DataManager
	{
	public:
		DoubleBuffVector<AttractorData>  attractorsData;
		DoubleBuffVector<AttractantData> attractantsData;
		DoubleBuffVector<PropulsedData>  propulsedData;

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
				LOG_ASSERT(false, "Specialize it for a specific type")
				return 0;
			}
			template<>
			size_t size<AttractorData>() const
			{
				return 7;
			}
			template<>
			size_t size<AttractantData>() const
			{
				return 6;
			}
			template<>
			size_t size<PropulsedData>() const
			{
				return 10;
			}

			template<typename DataType>
			void write(const DataType& data)
			{
				LOG_ASSERT(false, "Specialize it for a specific type")
			}
			template<>
			void write<AttractorData>(const AttractorData& data)
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
			void write<AttractantData>(const AttractantData& data)
			{
				for (uint8_t index = 0; index < 3; index++)
				{
					m_buffer[index + 0] = data.position[index];
					m_buffer[index + 3] = data.velocity[index];
				}
				std::fwrite(m_buffer.data(), sizeof(double), 7, p_file);
			}
			template<>
			void write<PropulsedData>(const PropulsedData& data)
			{
				for (uint8_t index = 0; index < 3; index++)
				{
					m_buffer[index + 0] = data.position[index];
					m_buffer[index + 3] = data.velocity[index];
					m_buffer[index + 6] = data.propulsion[index];
				}
				std::fwrite(m_buffer.data(), sizeof(double), 10, p_file);
			}

			template<typename DataType>
			void read(DataType& data)
			{
				LOG_ASSERT(false, "Specialize it for a specific type")
			}
			template<>
			void read<AttractorData>(AttractorData& data)
			{
				std::fread(m_buffer.data(), sizeof(double), 7, p_file);
				data.mass = m_buffer[0];
				data.position = { m_buffer[1], m_buffer[2], m_buffer[3] };
				data.velocity = { m_buffer[4], m_buffer[5], m_buffer[6] };
			}
			template<>
			void read<AttractantData>(AttractantData& data)
			{
				std::fread(m_buffer.data(), sizeof(double), 7, p_file);
				data.position = { m_buffer[0], m_buffer[1], m_buffer[2] };
				data.velocity = { m_buffer[3], m_buffer[4], m_buffer[5] };
			}
			template<>
			void read<PropulsedData>(PropulsedData& data)
			{
				std::fread(m_buffer.data(), sizeof(double), 10, p_file);
				data.position   = { m_buffer[0], m_buffer[1], m_buffer[2] };
				data.velocity   = { m_buffer[3], m_buffer[4], m_buffer[5] };
				data.propulsion = { m_buffer[6], m_buffer[7], m_buffer[8] };
			}
		};

	};
}