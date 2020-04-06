#pragma once
#include "Kx/Common.hpp"
#include <type_traits>
#include <utility>
#include <array>

namespace KxFramework::Utility
{
	template<class TLeft, class TRight>
	constexpr void ExchangeAndReset(TLeft& left, TLeft& right, TRight nullValue) noexcept
	{
		static_assert(std::is_trivially_move_assignable_v<TLeft> && std::is_trivially_move_assignable_v<TRight>,
					  "can only use ExchangeAndReset for trivially move assignable types");

		left = right;
		right = std::move(nullValue);
	}
	
	template<class TLeft, class TRight>
	constexpr TLeft ExchangeResetAndReturn(TLeft& right, TRight nullValue) noexcept
	{
		static_assert(std::is_default_constructible_v<TLeft>, "left type must be default constructible");

		TLeft left{};
		ExchangeAndReset(left, right, std::move(nullValue));
		return left;
	}

	template<class TFunc, class... Args>
	constexpr void ForEachParameterPackItem(TFunc&& func, Args&&... arg) noexcept
	{
		std::initializer_list<int>{(func(std::forward<Args>(arg)), 0)...};
	}

	template<class... Args>
	constexpr size_t CountOfParameterPack() noexcept
	{
		return sizeof...(Args);
	}

	template<class... Args>
	constexpr size_t SizeOfParameterPackValues() noexcept
	{
		const constexpr size_t count = CountOfParameterPack<Args...>();
		const constexpr std::array<size_t, count> sizes = {sizeof(Args)...};

		size_t sizeInBytes = 0;
		for (const size_t& size: sizes)
		{
			sizeInBytes += size;
		}
		return sizeInBytes;
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
}

namespace KxFramework::Utility
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

namespace KxFramework::Utility
{
	namespace Private
	{
		template<class T, bool isEnum = std::is_enum_v<T>, bool isInteger = std::is_integral_v<T>>
		struct UnderlyingType
		{
			using type = typename std::underlying_type<T>::type;
		};

		template<class T>
		struct UnderlyingType<T, false, true>
		{
			using type = typename T;
		};
	}
	namespace Private
	{
		template<class T>
		using FlagIntType = typename UnderlyingType<T>::type;

		template<class T>
		constexpr bool TestFlagType() noexcept
		{
			return std::is_integral_v<T> || std::is_enum_v<T>;
		}

		template<class T1, class T2>
		constexpr void AssertFlags() noexcept
		{
			static_assert(TestFlagType<T1>() && TestFlagType<T2>(), "flag must be integers or enums");
		}
	}

	template<class TFlag, class TFlagMod>
	constexpr TFlag ModFlag(TFlag flag, TFlagMod flagMod, bool set) noexcept
	{
		Private::AssertFlags<TFlag, TFlagMod>();

		using T1 = Private::FlagIntType<TFlag>;
		using T2 = Private::FlagIntType<TFlagMod>;
		using Tx = std::conditional_t<sizeof(T1) >= sizeof(T2), T1, T2>;

		if (set)
		{
			return static_cast<TFlag>(static_cast<Tx>(flag) | static_cast<Tx>(flagMod));
		}
		else
		{
			return static_cast<TFlag>(static_cast<Tx>(flag) & ~static_cast<Tx>(flagMod));
		}
	}

	template<class TFlag, class TFlagMod>
	constexpr void ModFlagRef(TFlag& flag, TFlagMod flagMod, bool set) noexcept
	{
		flag = ModFlag(flag, flagMod, set);
	}

	template<class TFlagLeft, class TFlagRight>
	constexpr bool HasFlag(TFlagLeft left, TFlagRight right) noexcept
	{
		Private::AssertFlags<TFlagLeft, TFlagRight>();

		using T1 = Private::FlagIntType<TFlagLeft>;
		using T2 = Private::FlagIntType<TFlagRight>;
		using Tx = std::conditional_t<sizeof(T1) >= sizeof(T2), T1, T2>;

		return static_cast<Tx>(left) & static_cast<Tx>(right) != static_cast<Tx>(0);
	}
}
