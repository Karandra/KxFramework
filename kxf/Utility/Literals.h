#pragma once
#include <cstdint>

namespace kxf
{
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0330r3.html
	constexpr size_t operator "" _zu(unsigned long long value) noexcept
	{
		return static_cast<size_t>(value);
	}
}
