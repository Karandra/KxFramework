#pragma once
#include "Common.h"

namespace kxf
{
	template<class T>
	class LockGuard final
	{
		private:
			T& m_Lock;

		public:
			LockGuard(T& lock)
				:m_Lock(lock)
			{
				m_Lock.Lock();
			}
			LockGuard(const LockGuard&) = delete;
			~LockGuard()
			{
				m_Lock.Unlock();
			}

		public:
			T* operator->() const noexcept
			{
				return &m_Lock;
			}

		public:
			LockGuard& operator=(const LockGuard&) = delete;
	};
}

namespace kxf
{
	template<class T>
	class ReadLockGuard final
	{
		private:
			T& m_Lock;

		public:
			ReadLockGuard(T& lock)
				:m_Lock(lock)
			{
				m_Lock.LockRead();
			}
			ReadLockGuard(const ReadLockGuard&) = delete;
			~ReadLockGuard()
			{
				m_Lock.UnlockRead();
			}

		public:
			T* operator->() const noexcept
			{
				return &m_Lock;
			}

		public:
			ReadLockGuard& operator=(const ReadLockGuard&) = delete;
	};

	template<class T>
	class WriteLockGuard final
	{
		private:
			T& m_Lock;

		public:
			WriteLockGuard(T& lock)
				:m_Lock(lock)
			{
				m_Lock.LockWrite();
			}
			WriteLockGuard(const WriteLockGuard&) = delete;
			~WriteLockGuard()
			{
				m_Lock.UnlockWrite();
			}

		public:
			T* operator->() const noexcept
			{
				return &m_Lock;
			}

		public:
			WriteLockGuard& operator=(const WriteLockGuard&) = delete;
	};
}
