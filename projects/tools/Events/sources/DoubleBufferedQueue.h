#pragma once

#include <queue>
#include <shared_mutex>

namespace projectSolar
{
	template<typename DataType>
	class DoubleBufferedQueue
	{
	public:
		template<typename ... Args>
		void emplace(const Args& ... args)
		{
			std::shared_lock stateLock(m_stateMutex);
			if (m_state)
			{
				std::unique_lock lock(m_mutex_0);
				m_events_0.emplace(args...);
			}
			else
			{
				std::unique_lock lock(m_mutex_1);
				m_events_1.emplace(args...);
			}
		}
		void push(const DataType& ev)
		{
			std::shared_lock stateLock(m_stateMutex);
			if (m_state)
			{
				std::unique_lock lock(m_mutex_0);
				m_events_0.emplace(ev);
			}
			else
			{
				std::unique_lock lock(m_mutex_1);
				m_events_1.emplace(ev);
			}
		}
		DataType pop()
		{
			std::shared_lock stateLock(m_stateMutex);
			if (m_state)
			{
				std::unique_lock lock(m_mutex_1);
				if (m_events_1.empty())
				{
					return DataType();
				}

				DataType ev = m_events_1.front();
				m_events_1.pop();
				return ev;
			}
			else
			{
				std::unique_lock lock(m_mutex_0);
				if (m_events_0.empty())
				{
					return DataType();
				}

				DataType ev = m_events_0.front();
				m_events_0.pop();
				return ev;
			}
		}
		void swap()
		{
			std::unique_lock lock(m_stateMutex);
			m_state = !m_state;
		}
		bool empty() const
		{
			std::shared_lock stateLock(m_stateMutex);
			if (m_state)
			{
				std::shared_lock lock(m_mutex_1);
				return m_events_1.empty();
			}
			else
			{
				std::shared_lock lock(m_mutex_0);
				return m_events_0.empty();
			}
		}
		size_t size() const
		{
			std::shared_lock stateLock(m_stateMutex);
			if (m_state)
			{
				std::shared_lock lock(m_mutex_1);
				return m_events_1.size();
			}
			else
			{
				std::shared_lock lock(m_mutex_0);
				return m_events_0.size();
			}
		}

	private:
		std::queue<DataType> m_events_0;
		std::queue<DataType> m_events_1;
		mutable std::shared_mutex m_mutex_0;
		mutable std::shared_mutex m_mutex_1;
		mutable std::shared_mutex m_stateMutex;
		bool m_state;
	};
}
