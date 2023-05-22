#pragma once

#include "DoubleBufferedContainer.h"
#include "Logger.h"

#include <stdio.h>
#include <Eigen/Eigen>
#include <Eigen/Sparse>

#include <array>
#include <vector>


namespace projectSolar::Simulation
{
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
			LOG_DEBUG("[Serializer] Try to open file: ", filePath.data());
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
			if (p_file != nullptr)
			{
				std::fclose(p_file);
			}
		}

		template<typename DataType>
		void serialize(DataStructures::DoubleBuffVector<DataType>& input)
		{
			serialize<DataType>(input.getData());
		}

		template<typename DataType>
		void serialize(std::vector<DataType>& input)
		{
			size_t dataSize = sizeof(DataType);
			size_t dataAmount = input.size();
			std::fwrite(&dataSize, sizeof(dataSize), 1, p_file);
			std::fwrite(&dataAmount, sizeof(dataAmount), 1, p_file);
			std::fwrite(input.data(), dataSize, dataAmount, p_file);
		}

		template<typename DataType>
		void deserialize(DataStructures::DoubleBuffVector<DataType>& input)
		{
			input.clear();

			size_t dataSize;
			size_t dataAmount;
			std::fread(&dataSize, sizeof(dataSize), 1, p_file);
			std::fread(&dataAmount, sizeof(dataAmount), 1, p_file);
			
			DataType buffer;
			input.reserve(dataAmount);
			for (auto index = 0; index < dataAmount; index++)
			{
				std::fread(&buffer, sizeof(DataType), 1, p_file);
				input.addElement(buffer);
			}
		}

		template<typename DataType>
		void deserialize(std::vector<DataType>& input)
		{
			input.clear();

			size_t dataSize;
			size_t dataAmount;
			std::fread(&dataSize, sizeof(dataSize), 1, p_file);
			std::fread(&dataAmount, sizeof(dataAmount), 1, p_file);

			input.resize(dataAmount);
			std::fread(input.data(), sizeof(DataType), dataAmount, p_file);
		}

	private:
		std::FILE* p_file = nullptr;
	};
}
