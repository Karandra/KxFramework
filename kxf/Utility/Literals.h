#pragma once
#include <cstdint>

namespace kxf::Literals
{
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p0330r8.html
	constexpr size_t operator"" _uz(unsigned long long value) noexcept
	{
		return static_cast<size_t>(value);
	}
}

namespace kxf
{
	using namespace Literals;
}
