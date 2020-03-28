#pragma once
#include "Kx/Common.hpp"
#include <type_traits>

namespace KxFramework::Utility
{
	template<class TLeft, class TRight>
	void ExchangeAndReset(TLeft& left, TLeft& right, TRight nullValue)
	{
		static_assert(std::is_trivially_move_assignable_v<TLeft> && std::is_trivially_move_assignable_v<TRight>,
					  "can only use ExchangeAndReset for trivially move assignable types");

		left = right;
		right = std::move(nullValue);
	}
	
	template<class TLeft, class TRight>
	TLeft ExchangeResetAndReturn(TLeft& right, TRight nullValue)
	{
		static_assert(std::is_default_constructible_v<TLeft>, "left type must be default constructible");

		TLeft left{};
		ExchangeAndReset(left, right, std::move(nullValue));
		return left;
	}

	template<class TFunc, class... Args>
	void ForEachParameterPackItem(TFunc&& func, Args&&... arg)
	{
		std::initializer_list<int>{(func(std::forward<Args>(arg)), 0)...};
	}

	template<class... Args>
	constexpr size_t CountOfParameterPack()
	{
		return sizeof...(Args);
	}

	template<class... Args>
	constexpr size_t SizeOfParameterPackValues()
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
	constexpr void SetIfNotNull(TPointer* ptr, TValue&& value)
	{
		using T = std::remove_reference<TValue>;
		static_assert(std::is_same_v<TPointer, T> || std::is_convertible_v<T, TPointer>, "incompatible types");

		if (ptr)
		{
			*ptr = static_cast<TPointer>(std::forward<TValue>(value));
		}
	}
}

namespace KxFramework::Utility
{
	namespace Internal
	{
		template<class T>
		using FlagIntType = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;

		template<class T>
		constexpr bool TestFlagType()
		{
			return std::is_integral_v<T> || std::is_enum_v<T>;
		}

		template<class T1, class T2>
		constexpr bool TestFlagsCompatibility()
		{
			using TInt1 = Internal::FlagIntType<T1>;
			using TInt2 = Internal::FlagIntType<T2>;

			return sizeof(T1) == sizeof(T2) && std::is_unsigned_v<TInt1> == std::is_unsigned_v<TInt2>;
		}

		template<class T1, class T2>
		constexpr void AssertFlags()
		{
			static_assert(TestFlagType<T1>() && TestFlagType<T2>(), "flag must be integers or enums");
			static_assert(TestFlagsCompatibility<T1, T2>(), "flags must be of the same size and sign");
		}
	}

	template<class TFlag, class TFlagMod>
	constexpr TFlag ModFlag(TFlag flag, TFlagMod flagMod, bool set)
	{
		Internal::AssertFlags<TFlag, TFlagMod>();

		using T1 = Internal::FlagIntType<TFlag>;
		using T2 = Internal::FlagIntType<TFlag>;

		if (set)
		{
			flag = static_cast<TFlag>(static_cast<T1>(flag) | static_cast<T2>(flagMod));
		}
		else
		{
			flag = static_cast<TFlag>(static_cast<T1>(flag) & ~static_cast<T2>(flagMod));
		}
		return flag;
	}

	template<class TFlag, class TFlagMod>
	constexpr void ModFlagRef(TFlag& flag, TFlagMod flagMod, bool set)
	{
		flag = ModFlag(flag, flagMod, set);
	}

	template<class TFlagLeft, class TFlagRight>
	constexpr bool HasFlag(TFlagLeft left, TFlagRight right)
	{
		Internal::AssertFlags<TFlagLeft, TFlagRight>();

		using T1 = Internal::FlagIntType<TFlagLeft>;
		using T2 = Internal::FlagIntType<TFlagRight>;
		return static_cast<T1>(left) & static_cast<T2>(right);
	}
}
