#pragma once
#include "kxf/Common.hpp"

namespace kxf::Threading
{
	KX_API uint32_t GetCurrentThreadID() noexcept;
	KX_API uint32_t GetHardwareConcurrency() noexcept;
}
