#pragma once
#include <type_traits>
#include <algorithm>
#include <cstdint>
#include <limits>

namespace kxf::Utility
{
	template<class T>
	constexpr std::enable_if_t<std::is_floating_point_v<T> || std::is_integral_v<T>, T> Abs(T value) noexcept
	{
		return value < static_cast<T>(0) ? -value : value;
	}

	template<class T>
	constexpr std::enable_if_t<std::is_floating_point_v<T>, T> Floor(T value) noexcept
	{
		return static_cast<T>(static_cast<int64_t>(value));
	}

	template<class T>
	constexpr std::enable_if_t<std::is_floating_point_v<T>, T> ModF(T value, T* integerPart = nullptr) noexcept
	{
		const int64_t intPart = static_cast<int64_t>(value);
		if (integerPart)
		{
			*integerPart = static_cast<T>(intPart);
		}

		return static_cast<T>(value - intPart);
	}

	template<class T>
	constexpr std::enable_if_t<std::is_floating_point_v<T>, bool> AlmostEqual(T left, T right, size_t units = 2) noexcept
	{
		// https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
		// The machine epsilon has to be scaled to the magnitude of the values used
		// and multiplied by the desired precision in ULPs (units in the last place),
		// unless the result is subnormal.

		const T sum = Abs(left + right);
		const T diff = Abs(left - right);
		return diff <= (std::numeric_limits<T>::epsilon() * sum * units) || diff < std::numeric_limits<T>::min();
	}

	template<class T>
	constexpr std::enable_if_t<std::is_floating_point_v<T>, bool> AlmostZero(T value, size_t units = 2) noexcept
	{
		return AlmostEqual(value, static_cast<T>(0), units);
	}

	template<class T>
	constexpr std::enable_if_t<std::is_integral_v<T>, bool> TestRange(T value, T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()) noexcept
	{
		return value == std::clamp(value, min, max);
	}

	template<class T>
	constexpr std::enable_if_t<std::is_floating_point_v<T>, bool> TestRange(T value, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max()) noexcept
	{
		return value == std::clamp(value, min, max);
	}
}
