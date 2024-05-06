#pragma once
#include "kxf/Common.hpp"
#include "TypeTraits.h"
#include <type_traits>
#include <algorithm>
#include <numeric>
#include <utility>
#include <array>

namespace kxf::Utility
{
	template<class T, class TNull> requires(std::is_move_assignable_v<T> && std::is_move_assignable_v<TNull>)
	constexpr void ExchangeAndReset(T& left, T& right, TNull&& nullValue) noexcept
	{
		left = std::move(right);
		right = std::move(nullValue);
	}
	
	template<class T, class TNull> requires(std::is_default_constructible_v<T>)
	constexpr T ExchangeResetAndReturn(T& right, TNull&& nullValue) noexcept
	{
		T left = std::move(right);
		right = std::move(nullValue);

		return left;
	}

	template<class TFunc, class... Args>
	constexpr void ForEachParameterPackItem(TFunc&& func, Args&&... arg) noexcept
	{
		std::initializer_list<int>{(std::invoke(func, std::forward<Args>(arg)), 0)...};
	}

	template<class... Args>
	constexpr size_t CountOfParameterPack() noexcept
	{
		return sizeof...(Args);
	}

	template<class... Args>
	constexpr bool IsEmptyParameterPack() noexcept
	{
		return CountOfParameterPack<Args...>() == 0;
	}

	template<class... Args>
	constexpr size_t SizeOfParameterPackValues() noexcept
	{
		const constexpr size_t count = CountOfParameterPack<Args...>();
		const constexpr std::array<size_t, count> sizes = {sizeof(Args)...};

		return std::accumulate(sizes.begin(), sizes.end(), static_cast<size_t>(0));
	}

	template<class TPointer, class TValue>
	constexpr void SetIfNotNull(TPointer* ptr, TValue&& value) noexcept
	{
		using T = std::remove_reference_t<TValue>;
		static_assert(std::is_same_v<TPointer, T> || std::is_convertible_v<T, TPointer>, "incompatible types");

		if (ptr)
		{
			*ptr = static_cast<TPointer>(std::forward<TValue>(value));
		}
	}

	template<class TPointer>
	std::unique_ptr<TPointer> AssumeUniquePtr(TPointer* ptr) noexcept
	{
		return std::unique_ptr<TPointer>(ptr);
	}

	template<class TTarget, class TSource>
	std::unique_ptr<TTarget> StaticCastUniquePtr(std::unique_ptr<TSource> source) noexcept
	{
		return std::unique_ptr<TTarget>(static_cast<TTarget*>(source.release()));
	}

	template<class TTarget, class TSource>
	std::unique_ptr<TTarget> DynamicCastUniquePtr(std::unique_ptr<TSource> source) noexcept
	{
		if (auto ptr = dynamic_cast<TTarget*>(source.get()))
		{
			std::unique_ptr<TTarget> result(ptr);
			source.release();

			return result;
		}
		return nullptr;
	}

	template<class T, class... Args> requires((std::is_integral_v<T> || std::is_enum_v<T>) && (std::is_enum_v<std::remove_const_t<std::remove_reference_t<Args>>> && ...))
	constexpr T CombineEnumFlags(Args&&... arg) noexcept
	{
		return static_cast<T>((static_cast<std::underlying_type_t<std::remove_const_t<std::remove_reference_t<Args>>>>(arg) | ...));
	}
}

namespace kxf::Utility
{
	template<typename>
	struct ArraySize;

	template<typename T, size_t N>
	struct ArraySize<T[N]>
	{
		static constexpr size_t value = N;
	};

	template<typename T, size_t N>
	struct ArraySize<std::array<T, N>>
	{
		static constexpr size_t value = N;
	};
}

namespace kxf::Utility
{
	namespace Private
	{
		template<class TOUT, bool highPart, class TIN>
		constexpr TOUT GetIntPart(TIN fullValue) noexcept
		{
			static_assert(std::is_integral_v<TIN> && std::is_integral_v<TOUT>, "only integral types allowed");
			static_assert(sizeof(TIN) == 2 * sizeof(TOUT), "sizeof(TIN) not equal to 2 * sizeof(TOUT)");

			union
			{
				struct
				{
					TOUT Low;
					TOUT High;
				};
				TIN Full;
			} value;
			value.Full = fullValue;

			return highPart ? value.High : value.Low;
		}
	}

	template<class TOUT, class TIN1, class TIN2>
	constexpr TOUT IntFromLowHigh(TIN1 low, TIN2 high) noexcept
	{
		static_assert(std::is_integral_v<TIN1> && std::is_integral_v<TIN2> && std::is_integral_v<TOUT>, "only integral types allowed");
		static_assert(sizeof(TIN1) == sizeof(TIN2), "sizeof(TIN1) not equal to sizeof(TIN2)");
		static_assert(sizeof(TOUT) == sizeof(TIN1) + sizeof(TIN2), "sizeof(TIN1) + sizeof(TIN2) not equal to sizeof(TOUT)");

		union
		{
			struct
			{
				TIN1 Low;
				TIN2 High;
			};
			TOUT Full;
		} value;
		
		value.Low = low;
		value.High = high;
		return value.Full;
	}

	template<class TOUT, class TIN>
	constexpr TOUT IntLowPart(TIN value) noexcept
	{
		return Private::GetIntPart<TOUT, false>(value);
	}

	template<class TOUT, class TIN>
	constexpr TOUT IntHighPart(TIN value) noexcept
	{
		return Private::GetIntPart<TOUT, true>(value);
	}
}
