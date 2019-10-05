/*
Copyright © 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxFormat.h"

namespace KxUtility::String
{
	using TStdWxString = typename std::basic_string<wxChar, std::char_traits<wxChar>, std::allocator<wxChar>>;
	using TStdWxStringView = typename std::basic_string_view<wxChar, std::char_traits<wxChar>>;

	inline TStdWxStringView ToWxStringView(const wxString& string)
	{
		return TStdWxStringView(string.wc_str(), string.length());
	}
	inline TStdWxStringView ToWxStringView(const TStdWxString& string)
	{
		return TStdWxStringView(string.c_str(), string.length());
	}
	inline wxString FromWxStringView(TStdWxStringView view)
	{
		return wxString(view.data(), view.length());
	}
}

namespace KxUtility::String
{
	template<class... Args> wxString Concat(Args&&... arg)
	{
		return (wxString(arg) + ...);
	}
	template<class... Args> wxString ConcatWithSeparator(const wxString& sep, Args&&... arg)
	{
		wxString value = ((wxString(arg) + sep) + ...);
		value.RemoveLast(sep.length());
		return value;
	}

	template<class TFunctor> void SplitBySeparator(const wxString& string, const wxString& sep, TFunctor&& func)
	{
		if (string.IsEmpty() || sep.IsEmpty())
		{
			func(ToWxStringView(string));
			return;
		}

		size_t pos = 0;
		size_t separatorPos = string.find(sep);
		if (separatorPos == wxString::npos)
		{
			separatorPos = string.length();
		}

		while (pos < string.length() && separatorPos <= string.length())
		{
			const TStdWxStringView stringPiece(string.wc_str() + pos, separatorPos - pos);
			if (!func(stringPiece))
			{
				return;
			}

			pos += stringPiece.length() + sep.length();
			separatorPos = string.find(sep, pos);

			// No separator found, but this is not the last element
			if (separatorPos == wxString::npos && pos < string.length())
			{
				separatorPos = string.length();
			}
		}
	}
	template<class TFunctor> void SplitByLength(const wxString& string, size_t length, TFunctor&& func)
	{
		const size_t stringLength = string.length();

		if (length != 0)
		{
			for (size_t i = 0; i < stringLength; i += length)
			{
				const TStdWxStringView stringPiece(string.wc_str() + i, std::min(length, stringLength - i));
				if (!func(stringPiece))
				{
					return;
				}
			}
		}
		else
		{
			func(ToWxStringView(string));
		}
	}
}
