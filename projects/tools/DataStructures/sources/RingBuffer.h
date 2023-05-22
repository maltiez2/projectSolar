#pragma once

#include <shared_mutex>

namespace projectSolar::DataStructures
{
	namespace RingBuffer
	{
		constexpr size_t MAX_BUFFER_SIZE = 1i64 << 25; // 16 Mb

		struct Buffer
		{
			uint8_t* front;
			uint8_t* back;
			uint8_t* head;
			uint8_t* tail;
		};

		Buffer* create(const size_t size);
		void destroy(Buffer* buffer);
		void clear(Buffer* buffer, std::shared_mutex* head, std::shared_mutex* tail);

		// Only POD data
		void put(Buffer* buffer, const uint8_t* data, const uint8_t dataType, const uint8_t size, std::shared_mutex* head, std::shared_mutex* tail);
		bool pop(Buffer* buffer, uint8_t* data, uint8_t& dataType, uint8_t& size, std::shared_mutex* head, std::shared_mutex* tail);

		bool splited(const Buffer* buffer, std::shared_mutex* head, std::shared_mutex* tail);
		size_t left(const Buffer* buffer, std::shared_mutex* head, std::shared_mutex* tail);
		bool empty(const Buffer* buffer, std::shared_mutex* head, std::shared_mutex* tail);
		bool full(const Buffer* buffer, std::shared_mutex* head, std::shared_mutex* tail);
	}
}