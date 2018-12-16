/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include <string>
#include <string_view>

namespace
{
	constexpr const wxChar DefaultFillChar = wxS(' ');
	constexpr const wxChar DefaultFloatFormat = wxS('f');

	template<class T> inline constexpr bool IsCharType = std::is_same_v<T, char> || std::is_same_v<T, wchar_t>;
	template<class T> inline constexpr bool IsCharPointer =
		std::is_same_v<T, char*> ||
		std::is_same_v<T, wchar_t*> ||
		std::is_same_v<T, const char*> ||
		std::is_same_v<T, const wchar_t*>;

	template<class T> inline constexpr bool FmtString =
		IsCharType<T> ||
		IsCharPointer<T> ||
		std::is_same_v<T, bool> ||
		std::is_same_v<T, std::string_view> ||
		std::is_same_v<T, std::wstring_view> ||
		std::is_constructible_v<wxString, T>;

	template<class T> inline constexpr bool FmtInteger = (std::is_integral_v<T> || std::is_enum_v<T>) && !(IsCharType<T> || IsCharPointer<T> || std::is_pointer_v<T> || std::is_same_v<T, bool>);
	template<class T> inline constexpr bool FmtFloat = std::is_floating_point_v<T>;
	template<class T> inline constexpr bool FmtPointer = std::is_pointer_v<T> && !IsCharPointer<T>;
}

class KX_API KxFormat
{
	private:
		wxString m_String;
		size_t m_CurrentArgument = 1;
		bool m_UpperCase = false;

	private:
		bool FindAndReplace(const wxString& string, const std::wstring_view& index, size_t startAt = 0);
		void FindAndReplace(const wxString& string, size_t index, size_t startAt = 0);
		void FindCurrentAndReplace(const wxString& string);

	private:
		// Strings
		void FormatString(const wxString& a, int fieldWidth, const wxUniChar& fillChar);
		void FormatChar(const wxUniChar& a, int fieldWidth, const wxUniChar& fillChar);

		// Integers
		void FormatInt(int8_t a, int fieldWidth, int base, const wxUniChar& fillChar);
		void FormatInt(uint8_t a, int fieldWidth, int base, const wxUniChar& fillChar);

		void FormatInt(int16_t a, int fieldWidth, int base, const wxUniChar& fillChar);
		void FormatInt(uint16_t a, int fieldWidth, int base, const wxUniChar& fillChar);

		void FormatInt(int32_t a, int fieldWidth, int base, const wxUniChar& fillChar);
		void FormatInt(uint32_t a, int fieldWidth, int base, const wxUniChar& fillChar);

		void FormatInt(int64_t a, int fieldWidth, int base, const wxUniChar& fillChar);
		void FormatInt(uint64_t a, int fieldWidth, int base, const wxUniChar& fillChar);

		void FormatPointer(const void* a, int fieldWidth, const wxUniChar& fillChar);
		void FormatBool(bool a, int fieldWidth, const wxUniChar& fillChar);

		// Floats
		void FormatDouble(double a, int precision, int fieldWidth, const wxUniChar& format, const wxUniChar& fillChar);

	public:
		KxFormat(const wxString& format)
			:m_String(format)
		{
		}

	public:
		wxString ToString() const
		{
			return m_String;
		}
		operator wxString() const
		{
			return m_String;
		}

		// Switches
		KxFormat& UpperCase(bool value = true)
		{
			m_UpperCase = value;
			return *this;
		}
		KxFormat& LowerCase(bool value = true)
		{
			m_UpperCase = !value;
			return *this;
		}

	public:
		template<class T> typename std::enable_if<FmtString<T>, KxFormat&>::type
		arg(const T& a, int fieldWidth = 0, const wxUniChar& fillChar = DefaultFillChar)
		{
			if constexpr(std::is_same_v<T, bool>)
			{
				FormatBool(a, fieldWidth, fillChar);
			}
			else if constexpr(std::is_constructible_v<wxUniChar, T>)
			{
				FormatChar(a, fieldWidth, fillChar);
			}
			else if constexpr(std::is_constructible_v<wxString, T>)
			{
				FormatString(a, fieldWidth, fillChar);
			}
			else if constexpr(std::is_same_v<T, std::string_view> || std::is_same_v<T, std::wstring_view>)
			{
				FormatString(wxString(a.data(), a.size()), fieldWidth, fillChar);
			}
			else
			{
				static_assert(false, "KxFormat: unsupported type for string formatting");
			}
			return *this;
		}
		
		template<class T> typename std::enable_if<FmtInteger<T>, KxFormat&>::type
		arg(const T& a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = DefaultFillChar)
		{
			if constexpr(std::is_integral_v<T>)
			{
				FormatInt(a, fieldWidth, base, fillChar);
			}
			else if constexpr(std::is_enum_v<T>)
			{
				FormatInt(static_cast<std::underlying_type_t<T>>(a), fieldWidth, base, fillChar);
			}
			else
			{
				static_assert(false, "KxFormat: unsupported type for integer formatting");
			}
			return *this;
		}

		template<class T> typename std::enable_if<FmtPointer<T>, KxFormat&>::type
		arg(const T& a, int fieldWidth = sizeof(void*) * 2, const wxUniChar& fillChar = wxS('0'))
		{
			if constexpr (std::is_pointer_v<T>)
			{
				FormatPointer(a, fieldWidth, fillChar);
			}
			else
			{
				static_assert(false, "KxFormat: unsupported type for pointer formatting");
			}
			return *this;
		}

		template<class T> typename std::enable_if<FmtFloat<T>, KxFormat&>::type
		arg(const T& a, int precision = -1, int fieldWidth = 0, const wxUniChar& format = DefaultFloatFormat, const wxUniChar& fillChar = DefaultFillChar)
		{
			if constexpr(std::is_floating_point_v<T>)
			{
				FormatDouble(a, precision, fieldWidth, format, fillChar);
			}
			else
			{
				static_assert(false, "KxFormat: unsupported type for floating-point formatting");
			}
			return *this;
		}
};
