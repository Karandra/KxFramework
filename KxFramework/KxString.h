/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"

class KxString
{
	public:
		// There values correspond to 'CompareStringOrdinal' results
		enum CompareResult
		{
			LessThan = 1,
			Equal = 2,
			GreaterThan = 3
		};

	private:
		KxString() = delete;
		KxString& operator=(KxString&) = delete;

	public:
		/* Upper/Lower */
		static char CharToLower(char c);
		static char CharToUpper(char c);
		static wchar_t CharToLower(wchar_t c);
		static wchar_t CharToUpper(wchar_t c);

		static wxUniChar& CharMakeLower(wxUniChar& c);
		static wxUniChar& CharMakeUpper(wxUniChar& c);

		static wxUniChar CharToLower(const wxUniChar& c)
		{
			wxUniChar temp(c);
			CharMakeLower(temp);
			return temp;
		}
		static wxUniChar CharToLower(const wxUniCharRef& c)
		{
			return CharToLower(wxUniChar(c));
		}
		static wxUniChar CharToUpper(const wxUniChar& c)
		{
			wxUniChar temp(c);
			CharMakeUpper(temp);
			return temp;
		}
		static wxUniChar CharToUpper(const wxUniCharRef& c)
		{
			return CharToUpper(wxUniChar(c));
		}

		static wxString& MakeLower(wxString& s);
		static wxString& MakeUpper(wxString& s);

		static wxString ToLower(const wxString& s)
		{
			wxString temp(s);
			MakeLower(temp);
			return temp;
		}
		static wxString ToUpper(const wxString& s)
		{
			wxString temp(s);
			MakeUpper(temp);
			return temp;
		}

		static wxString& MakeCapitalized(wxString& s, bool fistCharOnly = false);
		static wxString Capitalize(const wxString& s, bool fistCharOnly = false)
		{
			wxString temp(s);
			MakeCapitalized(temp);
			return temp;
		}

		/* Match and compare */
		static CompareResult Compare(const std::wstring_view& v1, const std::wstring_view& v2, bool ignoreCase = true);
		static CompareResult Compare(const wxString& v1, const wxString& v2, bool ignoreCase = true);

		static bool Matches(const std::string_view& string, const std::string_view& mask, bool ignoreCase = true);
		static bool Matches(const std::wstring_view& string, const std::wstring_view& mask, bool ignoreCase = true);
		static bool Matches(const wxString& string, const wxString& mask, bool ignoreCase = true);

		/* Split */
		static KxStringVector Split(const wxString& source, size_t partLength, bool allowEmpty = false);
		static KxStringVector Split(const wxString& source, const wxString& pattern, bool allowEmpty = false);

		/* Join */
		static wxString Join(const KxStringVector& stringList, const wxString& separator = wxEmptyString);

		/* Searching and replacing */
		static size_t Find(const wxString& source, const wxString& pattern, size_t startAt = 0, bool caseSensetivity = true);
		static size_t Replace(wxString& source, const wxString& pattern, const wxString& replacement, size_t startAt = 0, bool caseSensetivity = true);
		
		/* Misc */
		static wxString AbbreviateFilePath(const wxString& source, size_t maxChars);

		static wxString& Trim(wxString& source, bool left, bool right)
		{
			if (left)
			{
				source.Trim(false);
			}
			if (right)
			{
				source.Trim(true);
			}
			return source;
		}
		static wxString Trim(const wxString& source, bool left, bool right)
		{
			wxString temp(source);
			Trim(temp, left, right);
			return temp;
		}
};
