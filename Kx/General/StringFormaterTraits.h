#pragma once
#include "Common.h"
#include <wx/string.h>

namespace kxf
{
	class String;
}

namespace kxf::StringFormatter
{
	template<class T>
	class TypeTraits final
	{
		public:
			using TInitial = typename T;
			using TDecayed = typename std::decay<TInitial>::type;

		protected:
			template<class T, class... Args>
			constexpr static bool IsConvertibleToAnyOf() noexcept
			{
				return std::disjunction_v<std::is_convertible<T, Args>...>;
			}

		public:
			constexpr static bool IsConst() noexcept
			{
				return std::is_const_v<TInitial>;
			}
			constexpr static bool IsReference() noexcept
			{
				return std::is_reference_v<TInitial>;
			}

			constexpr static bool IsChar() noexcept
			{
				return std::is_same_v<TDecayed, char> || std::is_same_v<TDecayed, wchar_t>;
			}
			constexpr static bool IsCharPointer() noexcept
			{
				return (std::is_same_v<TDecayed, char*> || std::is_same_v<TDecayed, const char*>) ||
					(std::is_same_v<TDecayed, wchar_t*> || std::is_same_v<TDecayed, const wchar_t*>);
			}
			constexpr static bool IsCharArray() noexcept
			{
				return IsArray() && IsCharPointer();
			}
			constexpr static bool IsArray() noexcept
			{
				return std::is_array_v<TInitial>;
			}
			constexpr static bool IsBool() noexcept
			{
				return std::is_same_v<TDecayed, bool>;
			}
			constexpr static bool IsInteger() noexcept
			{
				return std::is_integral_v<TDecayed> || std::is_enum_v<TDecayed>;
			}
			constexpr static bool IsEnum() noexcept
			{
				return std::is_enum_v<TDecayed>;
			}
			constexpr static bool IsFloat()
			{
				return std::is_floating_point_v<TDecayed>;
			}
			constexpr static bool IsPointer() noexcept
			{
				return std::is_pointer_v<TDecayed>;
			}

			constexpr static bool IsStringView() noexcept
			{
				return std::is_same_v<TDecayed, std::string_view>;
			}
			constexpr static bool IsWStringView() noexcept
			{
				return std::is_same_v<TDecayed, std::wstring_view>;
			}
			constexpr static bool IsString() noexcept
			{
				return std::is_same_v<TDecayed, String>;
			}
			constexpr static bool IsWxString() noexcept
			{
				return std::is_same_v<TDecayed, wxString>;
			}
			constexpr static bool IsConstructibleToString() noexcept
			{
				return std::is_constructible_v<String, TInitial>;
			}
			constexpr static bool IsConstructibleToWxString() noexcept
			{
				return std::is_constructible_v<wxString, TInitial>;
			}
			constexpr static bool IsConstructibleToWxUniChar() noexcept
			{
				return std::is_constructible_v<wxUniChar, TInitial>;
			}

			constexpr static bool IsConvertibleToAnyInt() noexcept
			{
				return IsConvertibleToAnyOf<TDecayed,
					int8_t, int16_t, int32_t, int64_t,
					uint8_t, uint16_t, uint32_t, uint64_t>();
			}
			constexpr static bool IsConvertibleToAnyFloat() noexcept
			{
				return IsConvertibleToAnyOf<TDecayed, float, double>();
			}

		public:
			constexpr static bool FmtString() noexcept
			{
				return IsChar() ||
					IsCharPointer() ||
					IsCharArray() ||
					IsBool() ||
					IsStringView() ||
					IsWStringView() ||
					IsConstructibleToString() ||
					IsConstructibleToWxString();
			}
			constexpr static bool FmtInteger() noexcept
			{
				return IsInteger() && !(IsBool() || IsChar() || IsPointer());
			}
			constexpr static bool FmtFloat() noexcept
			{
				return IsFloat();
			}
			constexpr static bool FmtPointer() noexcept
			{
				return IsPointer() && !(IsCharPointer() || IsCharArray());
			}
	};
}

namespace kxf::StringFormatter
{
	class DefaultFormatTraits
	{
		public:
			constexpr static int StringFiledWidth() noexcept
			{
				return 0;
			}
			constexpr static wxChar StringFillChar() noexcept
			{
				return wxS(' ');
			}

			constexpr static int IntFiledWidth() noexcept
			{
				return 0;
			}
			constexpr static int IntBase() noexcept
			{
				return 10;
			}
			constexpr static wxChar IntFillChar() noexcept
			{
				return wxS(' ');
			}

			constexpr static int PtrFiledWidth() noexcept
			{
				return sizeof(void*) * 2;
			}
			constexpr static wxChar PtrFillChar() noexcept
			{
				return wxS('0');
			}

			constexpr static int FloatFiledWidth() noexcept
			{
				return 0;
			}
			constexpr static int FloatPrecision() noexcept
			{
				return -1;
			}
			constexpr static wxChar FloatFormat() noexcept
			{
				return wxS('f');
			}
			constexpr static wxChar FloatFillChar() noexcept
			{
				return wxS('0');
			}
	};
}
