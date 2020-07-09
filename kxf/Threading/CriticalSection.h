#pragma once
#include "Common.h"
#include "LockGuard.h"
#include "kxf/General/AlignedStorage.h"
struct _RTL_CRITICAL_SECTION;

namespace kxf
{
	class KX_API CriticalSection final
	{
		private:
			struct CritSec final
			{
				void* DebugInfo;
				uint32_t LockCount;
				uint32_t RecursionCount;
				void* OwningThread;
				void* LockSemaphore;
				size_t SpinCount;
			};
			AlignedBuffer<_RTL_CRITICAL_SECTION, sizeof(CritSec), alignof(CritSec)> m_CritSec;

		public:
			CriticalSection() noexcept;
			CriticalSection(uint32_t spinCount) noexcept;
			CriticalSection(const CriticalSection&) = delete;
			~CriticalSection() noexcept;

		public:
			void Enter() noexcept;
			bool TryEnter() noexcept;
			void Leave() noexcept;

			_RTL_CRITICAL_SECTION& GetHandle() noexcept
			{
				return *m_CritSec;
			}
			uint32_t SetSpinCount(uint32_t spinCount) noexcept;

		public:
			CriticalSection& operator=(const CriticalSection&) = delete;
	};
}

namespace kxf
{
	template<>
	class LockGuard<CriticalSection> final
	{
		private:
			CriticalSection& m_Lock;

		public:
			LockGuard(CriticalSection& lock) noexcept
				:m_Lock(lock)
			{
				m_Lock.Enter();
			}
			LockGuard(const LockGuard&) = delete;
			~LockGuard() noexcept
			{
				m_Lock.Leave();
			}

		public:
			CriticalSection* operator->() const noexcept
			{
				return &m_Lock;
			}

		public:
			LockGuard& operator=(const LockGuard&) = delete;
	};
}
