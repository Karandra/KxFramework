#pragma once
#include "Common.h"
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

			uint32_t SetSpinCount(uint32_t spinCount) noexcept;

		public:
			CriticalSection& operator=(const CriticalSection&) = delete;
	};
}
