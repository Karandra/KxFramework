/*
Copyright © 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include <string>
#include <string_view>

template<class Type> class KxFormatTypeTraits
{
	public:
		using TInitial = typename Type;
		using TDecayed = typename std::decay<TInitial>::type;

	protected:
		template<class T, class... Args> constexpr static bool IsConvertibleToAnyOf()
		{
			return std::disjunction_v<std::is_convertible<T, Args>...>;
		}

	public:
		constexpr static bool IsConst()
		{
			return std::is_const_v<TInitial>;
		}
		constexpr static bool IsReference()
		{
			return std::is_reference_v<TInitial>;
		}

		constexpr static bool IsChar()
		{
			return std::is_same_v<TDecayed, char> || std::is_same_v<TDecayed, wchar_t>;
		}
		constexpr static bool IsCharPointer()
		{
			return (std::is_same_v<TDecayed, char*> || std::is_same_v<TDecayed, const char*>) ||
				(std::is_same_v<TDecayed, wchar_t*> || std::is_same_v<TDecayed, const wchar_t*>);
		}
		constexpr static bool IsCharArray()
		{
			return IsArray() && IsCharPointer();
		}
		constexpr static bool IsArray()
		{
			return std::is_array_v<TInitial>;
		}
		constexpr static bool IsBool()
		{
			return std::is_same_v<TDecayed, bool>;
		}
		constexpr static bool IsInteger()
		{
			return std::is_integral_v<TDecayed> || std::is_enum_v<TDecayed>;
		}
		constexpr static bool IsEnum()
		{
			return std::is_enum_v<TDecayed>;
		}
		constexpr static bool IsFloat()
		{
			return std::is_floating_point_v<TDecayed>;
		}
		constexpr static bool IsPointer()
		{
			return std::is_pointer_v<TDecayed>;
		}

		constexpr static bool IsStringView()
		{
			return std::is_same_v<TDecayed, std::string_view>;
		}
		constexpr static bool IsWStringView()
		{
			return std::is_same_v<TDecayed, std::wstring_view>;
		}
		constexpr static bool IsWxString()
		{
			return std::is_same_v<TDecayed, wxString>;
		}
		constexpr static bool IsConstructibleToWxString()
		{
			return std::is_constructible_v<wxString, TInitial>;
		}
		constexpr static bool IsConstructibleToWxUniChar()
		{
			return std::is_constructible_v<wxUniChar, TInitial>;
		}

		constexpr static bool IsConvertibleToAnyInt()
		{
			return IsConvertibleToAnyOf<TDecayed,
				int8_t, int16_t, int32_t, int64_t,
				uint8_t, uint16_t, uint32_t, uint64_t>();
		}
		constexpr static bool IsConvertibleToAnyFloat()
		{
			return IsConvertibleToAnyOf<TDecayed, float, double>();
		}

	public:
		constexpr static bool FmtString()
		{
			return IsChar() || IsCharPointer() || IsCharArray() || IsBool() || IsStringView() || IsWStringView() || IsConstructibleToWxString();
		}
		constexpr static bool FmtInteger()
		{
			return (IsInteger() && !(IsBool() || IsChar() || IsPointer()));
		}
		constexpr static bool FmtFloat()
		{
			return IsFloat();
		}
		constexpr static bool FmtPointer()
		{
			return IsPointer() && !(IsCharPointer() || IsCharArray());
		}
};

class KxFormatTraits
{
	public:
		constexpr static int StringFiledWidth()
		{
			return 0;
		}
		constexpr static wxChar StringFillChar()
		{
			return wxS(' ');
		}

		constexpr static int IntFiledWidth()
		{
			return 0;
		}
		constexpr static int IntBase()
		{
			return 10;
		}
		constexpr static wxChar IntFillChar()
		{
			return wxS(' ');
		}

		constexpr static int PtrFiledWidth()
		{
			return sizeof(void*) * 2;
		}
		constexpr static wxChar PtrFillChar()
		{
			return wxS('0');
		}

		constexpr static int FloatFiledWidth()
		{
			return 0;
		}
		constexpr static int FloatPrecision()
		{
			return -1;
		}
		constexpr static wxChar FloatFormat()
		{
			return wxS('f');
		}
		constexpr static wxChar FloatFillChar()
		{
			return wxS('0');
		}
};
