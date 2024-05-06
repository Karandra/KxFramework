#pragma once
#include "kxf/Serialization/Common.h"
#include <type_traits>

namespace kxf::XDocument::Private
{
	template<class T>
	constexpr bool TestIntType() noexcept
	{
		return std::is_integral_v<std::remove_cvref_t<T>> || std::is_enum_v<std::remove_cvref_t<T>>;
	}

	template<class T>
	constexpr bool TestFloatType() noexcept
	{
		return std::is_floating_point_v<std::remove_cvref_t<T>>;
	}

	template<class T>
	constexpr bool TestPtrType() noexcept
	{
		return std::is_pointer_v<std::remove_cvref_t<T>>;
	}
}
