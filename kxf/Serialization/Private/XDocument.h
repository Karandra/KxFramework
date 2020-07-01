#pragma once
#include "kxf/Serialization/Common.h"
#include <type_traits>

namespace kxf::XDocument::Private
{
	template<class T>
	using RemoveRCV = std::remove_cv_t<std::remove_reference_t<T>>;

	template<class T>
	constexpr bool TestIntType() noexcept
	{
		return std::is_integral_v<RemoveRCV<T>> || std::is_enum_v<RemoveRCV<T>>;
	}

	template<class T>
	constexpr bool TestFloatType() noexcept
	{
		return std::is_floating_point_v<RemoveRCV<T>>;
	}

	template<class T>
	constexpr bool TestPtrType() noexcept
	{
		return std::is_pointer_v<RemoveRCV<T>>;
	}

	template<class T>
	constexpr void AssertIntType() noexcept
	{
		static_assert(TestIntType<T>(), "T must be of integral or enum type");
	}

	template<class T>
	constexpr void AssertFloatType() noexcept
	{
		static_assert(TestFloatType<T>(), "T must be floating point number");
	}

	template<class T>
	constexpr void AssertPtrType() noexcept
	{
		static_assert(TestPtrType<T>(), "T must be a pointer");
	}
}
