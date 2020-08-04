#pragma once
#include "Common.h"

namespace kxf
{
	template<class T>
	class LockGuard final
	{
		private:
			T* m_Lock = nullptr;

		public:
			explicit LockGuard(T& lock)
				:m_Lock(&lock)
			{
				m_Lock->Lock();
			}
			LockGuard(LockGuard&& other) noexcept
			{
				*this = std::move(other);
			}
			LockGuard(const LockGuard&) = delete;
			~LockGuard()
			{
				Unlock();
			}

		public:
			void Unlock()
			{
				if (T* lock = m_Lock)
				{
					m_Lock = nullptr;
					lock->Unlock();
				}
			}
			T* operator->() const noexcept
			{
				return m_Lock;
			}

		public:
			LockGuard& operator=(LockGuard&& other) noexcept
			{
				m_Lock = other.m_Lock;
				other.m_Lock = nullptr;

				return *this;
			}
			LockGuard& operator=(const LockGuard&) = delete;
	};
}

namespace kxf
{
	template<class T>
	class ReadLockGuard final
	{
		private:
			T* m_Lock = nullptr;

		public:
			explicit ReadLockGuard(T& lock)
				:m_Lock(&lock)
			{
				m_Lock->LockRead();
			}
			ReadLockGuard(ReadLockGuard&& other) noexcept
			{
				*this = std::move(other);
			}
			ReadLockGuard(const ReadLockGuard&) = delete;
			~ReadLockGuard()
			{
				Unlock();
			}

		public:
			void Unlock()
			{
				if (T* lock = m_Lock)
				{
					m_Lock = nullptr;
					lock->UnlockRead();
				}
			}
			T* operator->() const noexcept
			{
				return m_Lock;
			}

		public:
			ReadLockGuard& operator=(const ReadLockGuard&) = delete;
			ReadLockGuard& operator=(ReadLockGuard&& other) noexcept
			{
				m_Lock = other.m_Lock;
				other.m_Lock = nullptr;

				return *this;
			}
	};

	template<class T>
	class WriteLockGuard final
	{
		private:
			T* m_Lock = nullptr;

		public:
			explicit WriteLockGuard(T& lock)
				:m_Lock(&lock)
			{
				m_Lock->LockWrite();
			}
			WriteLockGuard(WriteLockGuard&& other) noexcept
			{
				*this = std::move(other);
			}
			WriteLockGuard(const WriteLockGuard&) = delete;
			~WriteLockGuard()
			{
				Unlock();
			}

		public:
			void Unlock()
			{
				if (T* lock = m_Lock)
				{
					m_Lock = nullptr;
					lock->UnlockWrite();
				}
			}
			T* operator->() const noexcept
			{
				return m_Lock;
			}

		public:
			WriteLockGuard& operator=(WriteLockGuard&& other) noexcept
			{
				m_Lock = other.m_Lock;
				other.m_Lock = nullptr;

				return *this;
			}
			WriteLockGuard& operator=(const WriteLockGuard&) = delete;
	};
}
