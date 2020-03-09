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

	inline wxStringView ToStringView(const wxString& string)
	{
		return wxStringView(string.wc_str(), string.length());
	}
	inline wxStringView ToStringView(const TStdWxString& string)
	{
		return wxStringView(string.c_str(), string.length());
	}
	
	inline wxString FromStringView(std::string_view view)
	{
		return wxString::FromUTF8(view.data(), view.length());
	}
	inline wxString FromStringView(std::wstring_view view)
	{
		return wxString(view.data(), view.length());
	}
}

namespace KxUtility::String
{
	template<class... Args>
	wxString Concat(Args&&... arg)
	{
		return (wxString(arg) + ...);
	}
	
	template<class... Args>
	wxString ConcatWithSeparator(const wxString& sep, Args&&... arg)
	{
		wxString value = ((wxString(arg) + sep) + ...);
		value.RemoveLast(sep.length());
		return value;
	}

	template<class TFunctor>
	size_t SplitBySeparator(const wxString& string, const wxString& sep, TFunctor&& func)
	{
		if (sep.empty() && !string.empty())
		{
			func(ToStringView(string));
			return 1;
		}

		size_t separatorPos = string.find(sep);
		if (separatorPos == wxStringView::npos)
		{
			func(ToStringView(string));
			return 1;
		}

		size_t pos = 0;
		size_t count = 0;
		while (pos < string.length() && separatorPos <= string.length())
		{
			wxStringView stringPiece(string.wx_str() + pos, separatorPos - pos);
			const size_t stringPieceLength = stringPiece.length();

			if (!stringPiece.empty())
			{
				count++;
				if (!func(std::move(stringPiece)))
				{
					return count;
				}
			}

			pos += stringPieceLength + sep.length();
			separatorPos = string.find(sep, pos);

			// No separator found, but this is not the last element
			if (separatorPos == wxStringView::npos && pos < string.length())
			{
				separatorPos = string.length();
			}
		}
		return count;
	}
	
	template<class TFunctor>
	size_t SplitByLength(const wxString& string, size_t length, TFunctor&& func)
	{
		if (length != 0)
		{
			const size_t stringLength = string.length();

			size_t count = 0;
			for (size_t i = 0; i < stringLength; i += length)
			{
				wxStringView stringPiece = string.substr(i, length);
				if (!stringPiece.empty())
				{
					count++;
					if (!func(std::move(stringPiece)))
					{
						return count;
					}
				}
			}
			return count;
		}
		else
		{
			func(ToStringView(string));
			return 1;
		}
		return 0;
	}
}
