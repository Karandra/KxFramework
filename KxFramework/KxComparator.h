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

namespace KxComparator
{
	struct StringEqualToNoCase
	{
		bool operator()(const wxString& left, const wxString& right) const
		{
			return IsEqual(left, right, true);
		}
		bool operator()(std::wstring_view left, std::wstring_view right) const
		{
			return IsEqual(left, right, true);
		}
	};
	struct StringHashNoCase
	{
		// From Boost
		template<class T> static void hash_combine(size_t& seed, const T& v)
		{
			std::hash<T> hasher;
			seed ^= hasher(v) + size_t(0x9e3779b9u) + (seed << 6) + (seed >> 2);
		}

		size_t operator()(const wxString& value) const
		{
			size_t hashValue = 0;
			for (wxChar c: value)
			{
				hash_combine(hashValue, KxString::CharToLower(c));
			}
			return hashValue;
		}
		size_t operator()(std::wstring_view value) const
		{
			size_t hashValue = 0;
			for (wchar_t c: value)
			{
				hash_combine(hashValue, KxString::CharToLower(c));
			}
			return hashValue;
		}
	};

	template<class TKey, class TValue> using UMapNoCase = std::unordered_map<TKey, TValue, StringHashNoCase, StringEqualToNoCase>;
	template<class TValue> using USetNoCase = std::unordered_set<TValue, StringHashNoCase, StringEqualToNoCase>;
}
