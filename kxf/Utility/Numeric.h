#pragma once
#include <type_traits>
#include <algorithm>
#include <cstdint>
#include <limits>

namespace kxf::Utility
{
	template<class T> requires(std::is_arithmetic_v<T>)
	constexpr T Abs(T value) noexcept
	{
		return value < static_cast<T>(0) ? -value : value;
	}

	template<class T> requires(std::is_floating_point_v<T>)
	constexpr T Floor(T value) noexcept
	{
		return static_cast<T>(static_cast<int64_t>(value));
	}

	template<class T> requires(std::is_floating_point_v<T>)
	constexpr T ModF(T value, T* integerPart = nullptr) noexcept
	{
		const int64_t intPart = static_cast<int64_t>(value);
		if (integerPart)
		{
			*integerPart = static_cast<T>(intPart);
		}

		return static_cast<T>(value - intPart);
	}

	template<class T> requires(std::is_floating_point_v<T>)
	constexpr bool AlmostEqual(T left, T right, size_t units = 2) noexcept
	{
		// https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
		// The machine epsilon has to be scaled to the magnitude of the values used
		// and multiplied by the desired precision in ULPs (units in the last place),
		// unless the result is subnormal.

		const T sum = Abs(left + right);
		const T diff = Abs(left - right);
		return diff <= (std::numeric_limits<T>::epsilon() * sum * units) || diff < std::numeric_limits<T>::min();
	}

	template<class T> requires(std::is_floating_point_v<T>)
	constexpr bool AlmostZero(T value, size_t units = 2) noexcept
	{
		return AlmostEqual(value, static_cast<T>(0), units);
	}

	template<class T> requires(std::is_integral_v<T>)
	constexpr bool TestRange(T value, T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()) noexcept
	{
		return value == std::clamp(value, min, max);
	}

	template<class T> requires(std::is_floating_point_v<T>)
	constexpr bool TestRange(T value, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max()) noexcept
	{
		return value == std::clamp(value, min, max);
	}
}

namespace kxf::Utility
{
	template<class T> requires(std::is_integral_v<T>)
	constexpr T FirstDecimalDigit(T quantity) noexcept
	{
		while (quantity >= static_cast<T>(10))
		{
			quantity /= static_cast<T>(10);
		}
		return quantity;
	}

	template<class T> requires(std::is_integral_v<T>)
	constexpr T LastDecimalDigit(T quantity) noexcept
	{
		return quantity % static_cast<T>(10);
	}
}
