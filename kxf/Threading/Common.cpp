#include "KxfPCH.h"
#include "Common.h"
#include <Windows.h>

namespace kxf::Threading
{
	uint32_t GetCurrentThreadID() noexcept
	{
		return ::GetCurrentThreadId();
	}
}
