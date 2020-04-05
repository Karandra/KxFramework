#include "KxStdAfx.h"
#include "String.h"

namespace
{
	char CharToLower(char c)
	{
		#pragma warning(suppress: 4312)
		#pragma warning(suppress: 4302)
		return reinterpret_cast<char>(::CharLowerA(reinterpret_cast<LPSTR>(c)));
	}
	char CharToUpper(char c)
	{
		#pragma warning(suppress: 4312)
		#pragma warning(suppress: 4302)
		return reinterpret_cast<char>(::CharUpperA(reinterpret_cast<LPSTR>(c)));
	}
	wchar_t CharToLower(wchar_t c)
	{
		#pragma warning(suppress: 4312)
		#pragma warning(suppress: 4302)
		return reinterpret_cast<wchar_t>(::CharLowerW(reinterpret_cast<LPWSTR>(c)));
	}
	wchar_t CharToUpper(wchar_t c)
	{
		#pragma warning(suppress: 4312)
		#pragma warning(suppress: 4302)
		return reinterpret_cast<wchar_t>(::CharUpperW(reinterpret_cast<LPWSTR>(c)));
	}

	int CompareStrings(KxFramework::StringView left, KxFramework::StringView right, bool ignoreCase) noexcept
	{
		if (ignoreCase)
		{
			constexpr size_t maxLength = std::numeric_limits<int>::max();
			wxASSERT_MSG(left.length() <= maxLength && right.length() <= maxLength, __FUNCTION__ ": strings are too long to be compared using 'CompareStringOrdinal'");

			switch (::CompareStringOrdinal(left.data(), left.length(), right.data(), right.length(), ignoreCase))
			{
				case CSTR_LESS_THAN:
				{
					return -1;
				}
				case CSTR_EQUAL:
				{
					return 0;
				}
				case CSTR_GREATER_THAN:
				{
					return 1;
				}
			};
			return -2;
		}
		else
		{
			return left.compare(right);
		}
	}
}

namespace KxFramework
{
	int String::Compare(StringView left, StringView right, StrCmpFlag flags) noexcept
	{
		return CompareStrings(left, right, flags & StrCmpFlag::IgnoreCase);
	}
}
