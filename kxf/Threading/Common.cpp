#include "KxfPCH.h"
#include "Common.h"
#include <thread>
#include <Windows.h>

namespace kxf::Threading
{
	uint32_t GetCurrentThreadID() noexcept
	{
		return ::GetCurrentThreadId();
	}
	uint32_t GetHardwareConcurrency() noexcept
	{
		return std::thread::hardware_concurrency();
	}
}
