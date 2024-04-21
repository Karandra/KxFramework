#pragma once
#include "Common.h"
#include "IThreadPool.h"
#include <thread>
#include <mutex>
#include <queue>

namespace kxf
{
	class KX_API ThreadPool final: public RTTI::DynamicImplementation<ThreadPool, IThreadPool>
	{
		private:
			std::vector<std::thread> m_ThreadPool;
			mutable std::mutex m_ThreadPoolLock;

			std::queue<std::move_only_function<void()>> m_TaskQueue;
			std::condition_variable m_TaskCondition;
			mutable std::mutex m_TaskQueueLock;

			size_t m_Concurrency = 0;
			std::atomic<bool> m_ShouldTerminate = false;

		public:
			ThreadPool() noexcept
				:m_Concurrency(std::thread::hardware_concurrency())
			{
			}
			ThreadPool(size_t concurrency) noexcept
				:m_Concurrency(concurrency)
			{
			}
			ThreadPool(const ThreadPool&) = delete;
			~ThreadPool()
			{
				Terminate();
			}

		public:
			bool IsRunning() const override;
			bool ShouldTerminate() const override;

			size_t GetConcurrency() const override;
			bool SetConcurrency(size_t concurrency) override;

			void Run() override;
			void Terminate() override;
			void QueueTask(std::move_only_function<void()> task) override;

		public:
			ThreadPool& operator=(const ThreadPool&) = delete;
	};
}
