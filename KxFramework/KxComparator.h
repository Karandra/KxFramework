/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxString.h"

namespace KxComparator
{
	using CompareResult = KxString::CompareResult;

	// ?*
	template<class T> bool Matches(const T& string, const T& mask, bool ignoreCase = true)
	{
		return KxString::Matches(string, mask, ignoreCase);
	}
	inline bool Matches(const wxString& string, const wxString& mask, bool ignoreCase = true)
	{
		return KxString::Matches(string, mask, ignoreCase);
	}

	// ==
	template<class T> bool IsEqual(const T& v1, const T& v2)
	{
		return v1 == v2;
	}
	inline bool IsEqual(const wxString& v1, const wxString& v2, bool ignoreCase = true)
	{
		return KxString::Compare(v1, v2, ignoreCase) == CompareResult::Equal;
	}
	inline bool IsEqual(const std::wstring_view& v1, const std::wstring_view& v2, bool ignoreCase = true)
	{
		return KxString::Compare(v1, v2, ignoreCase) == CompareResult::Equal;
	}

	// <
	template<class T> bool IsLess(const T& v1, const T& v2)
	{
		return v1 < v2;
	}
	inline bool IsLess(const wxString& v1, const wxString& v2, bool ignoreCase = true)
	{
		return KxString::Compare(v1, v2, ignoreCase) == CompareResult::LessThan;
	}
	inline bool IsLess(const std::wstring_view& v1, const std::wstring_view& v2, bool ignoreCase = true)
	{
		return KxString::Compare(v1, v2, ignoreCase) == CompareResult::LessThan;
	}

	// <=
	template<class T> bool IsLessEqual(const T& v1, const T& v2)
	{
		return v1 <= v2;
	}
	inline bool IsLessEqual(const wxString& v1, const wxString& v2, bool ignoreCase = true)
	{
		return KxString::Compare(v1, v2, ignoreCase) == CompareResult::LessThan || KxString::Compare(v1, v2, ignoreCase) == CompareResult::Equal;
	}
	inline bool IsLessEqual(const std::wstring_view& v1, const std::wstring_view& v2, bool ignoreCase = true)
	{
		return KxString::Compare(v1, v2, ignoreCase) == CompareResult::LessThan || KxString::Compare(v1, v2, ignoreCase) == CompareResult::Equal;
	}

	// >
	template<class T> bool IsGreater(const T& v1, const T& v2)
	{
		return v1 > v2;
	}
	inline bool IsGreater(const wxString& v1, const wxString& v2, bool ignoreCase = true)
	{
		return KxString::Compare(v1, v2, ignoreCase) == CompareResult::GreaterThan;
	}
	inline bool IsGreater(const std::wstring_view& v1, const std::wstring_view& v2, bool ignoreCase = true)
	{
		return KxString::Compare(v1, v2, ignoreCase) == CompareResult::GreaterThan;
	}

	// >=
	template<class T> bool IsGreaterEqual(const T& v1, const T& v2)
	{
		return v1 >= v2;
	}
	inline bool IsGreaterEqual(const wxString& v1, const wxString& v2, bool ignoreCase = true)
	{
		return KxString::Compare(v1, v2, ignoreCase) == CompareResult::GreaterThan || KxString::Compare(v1, v2, ignoreCase) == CompareResult::Equal;
	}
	inline bool IsGreaterEqual(const std::wstring_view& v1, const std::wstring_view& v2, bool ignoreCase = true)
	{
		return KxString::Compare(v1, v2, ignoreCase) == CompareResult::GreaterThan || KxString::Compare(v1, v2, ignoreCase) == CompareResult::Equal;
	}
}
