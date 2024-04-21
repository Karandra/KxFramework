#include "KxfPCH.h"
#include "ThreadPool.h"

namespace kxf
{
	bool ThreadPool::IsRunning() const
	{
		if (std::unique_lock lock(m_ThreadPoolLock); m_ThreadPool.empty())
		{
			return true;
		}
		return false;
	}
	bool ThreadPool::ShouldTerminate() const
	{
		return m_ShouldTerminate;
	}

	size_t ThreadPool::GetConcurrency() const
	{
		return m_Concurrency;
	}
	bool ThreadPool::SetConcurrency(size_t concurrency)
	{
		if (std::unique_lock lock(m_ThreadPoolLock); m_ThreadPool.empty())
		{
			m_Concurrency = concurrency;
			return true;
		}
		return false;
	}

	void ThreadPool::Run()
	{
		if (std::unique_lock lock(m_ThreadPoolLock); true)
		{
			m_ThreadPool.reserve(m_Concurrency);
			for (size_t i = 0; i < m_Concurrency; i++)
			{
				m_ThreadPool.emplace_back([this]()
				{
					bool shouldTerminate = false;
					while (!shouldTerminate)
					{
						std::move_only_function<void()> task;
						if (std::unique_lock lock(m_TaskQueueLock); true)
						{
							m_TaskCondition.wait(lock, [&]()
							{
								shouldTerminate = m_ShouldTerminate;
								return !m_TaskQueue.empty() || shouldTerminate;
							});

							if (!shouldTerminate)
							{
								task = std::move(m_TaskQueue.front());
								m_TaskQueue.pop();
							}
						}

						if (task)
						{
							std::invoke(task);
						}
					}
				});
			}
		}
	}
	void ThreadPool::Terminate()
	{
		if (std::unique_lock lock(m_ThreadPoolLock); !m_ThreadPool.empty())
		{
			m_ShouldTerminate = true;

			// Wake up all threads
			m_TaskCondition.notify_all();

			// Join all threads
			for (auto& thread: m_ThreadPool)
			{
				thread.join();
			}
			m_ThreadPool.clear();
			m_ShouldTerminate = false;
		}
	}
	void ThreadPool::QueueTask(std::move_only_function<void()> task)
	{
		if (task)
		{
			if (std::unique_lock lock(m_TaskQueueLock); true)
			{
				m_TaskQueue.emplace(std::move(task));
			}
			m_TaskCondition.notify_one();
		}
	}
}
