#pragma once
#include "Common.h"

namespace KxFramework::EnumClass
{
	template<class T>
	struct IsCastAllowed: std::false_type {};

	template<class T>
	inline constexpr bool IsCastAllowed_v = IsCastAllowed<T>::value;

	template<class T>
	struct IsBitwiseAllowed: std::false_type {};
	
	template<class T>
	inline constexpr bool IsBitwiseAllowed_v = IsBitwiseAllowed<T>::value;

	#define Kx_EnumClass_AllowCast(T) template<> struct IsCastAllowed<T>: std::true_type {}
	#define Kx_EnumClass_AllowBitwise(T) template<> struct IsBitwiseAllowed<T>: std::true_type {}

	#define Kx_EnumClass_AllowEverything(T)	\
		Kx_EnumClass_AllowCast(T);	\
		Kx_EnumClass_AllowBitwise(T)
}

namespace KxFramework::EnumClass::Private
{
	template<class T>
	inline constexpr bool IntCastAllowed = IsCastAllowed_v<T> || IsBitwiseAllowed_v<T>;

	template<class TEnum>
	class ValueWrapper final
	{
		private:
			TEnum m_Value;

		private:
			constexpr auto AsInt() const noexcept
			{
				return static_cast<std::underlying_type_t<TEnum>>(m_Value);
			}

		public:
			constexpr ValueWrapper() = default;
			constexpr ValueWrapper(TEnum value) noexcept
				:m_Value(value)
			{
			}
			
		public:
			constexpr operator TEnum() const noexcept
			{
				return m_Value;
			}
			constexpr operator bool() const noexcept
			{
				return AsInt() != 0;
			}
			constexpr bool operator!() const noexcept
			{
				return AsInt() == 0;
			}
	};
}

namespace KxFramework
{
	template<class TEnum>
	constexpr std::enable_if_t<EnumClass::Private::IntCastAllowed<TEnum>, bool> ToBool(TEnum value)
	{
		using Tint = std::underlying_type_t<TEnum>;
		return static_cast<Tint>(value) != 0;
	}

	template<class TInt, class TEnum>
	constexpr std::enable_if_t<EnumClass::Private::IntCastAllowed<TEnum>, TInt> ToInt(TEnum value)
	{
		return static_cast<TInt>(value);
	}

	template<class TEnum, class TInt = std::underlying_type_t<TEnum>>
	constexpr std::enable_if_t<EnumClass::Private::IntCastAllowed<TEnum>, TInt> ToInt(TEnum value)
	{
		return static_cast<TInt>(value);
	}

	template<class TEnum, class TInt>
	constexpr std::enable_if_t<EnumClass::Private::IntCastAllowed<TEnum>, TEnum> FromInt(TInt value)
	{
		return static_cast<TEnum>(value);
	}
}

namespace KxFramework
{
	template<class TEnum>
	constexpr std::enable_if_t<EnumClass::IsBitwiseAllowed_v<TEnum>, EnumClass::Private::ValueWrapper<TEnum>> operator&(TEnum left, TEnum right)
	{
		using Tint = std::underlying_type_t<TEnum>;
		return static_cast<TEnum>(static_cast<Tint>(left) & static_cast<Tint>(right));
	}

	template<class TEnum>
	constexpr std::enable_if_t<EnumClass::IsBitwiseAllowed_v<TEnum>, TEnum> operator|(TEnum left, TEnum right)
	{
		using Tint = std::underlying_type_t<TEnum>;
		return static_cast<TEnum>(static_cast<Tint>(left) | static_cast<Tint>(right));
	}

	template<class TEnum>
	constexpr std::enable_if_t<EnumClass::IsBitwiseAllowed_v<TEnum>, TEnum&> operator&=(TEnum& left, TEnum right)
	{
		left = left & right;
		return left;
	}

	template<class TEnum>
	constexpr std::enable_if_t<EnumClass::IsBitwiseAllowed_v<TEnum>, TEnum&> operator|=(TEnum& left, TEnum right)
	{
		left = left | right;
		return left;
	}

	template<class TEnum>
	constexpr std::enable_if_t<EnumClass::IsBitwiseAllowed_v<TEnum>, TEnum> operator~(TEnum value)
	{
		using Tint = std::underlying_type_t<TEnum>;
		return static_cast<TEnum>(~static_cast<Tint>(value));
	}
}
