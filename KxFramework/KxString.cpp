/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxString.h"
#include <Shlwapi.h>
#include "KxWinUndef.h"

namespace Util
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
		return reinterpret_cast<char>(::CharLowerA(reinterpret_cast<LPSTR>(c)));
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
		return reinterpret_cast<wchar_t>(::CharLowerW(reinterpret_cast<LPWSTR>(c)));
	}

	// Check whether Name matches Expression
	// Expression can contain "?"(any one character) and "*" (any string)
	// when IgnoreCase is true, do case insensitive matching
	//
	// http://msdn.microsoft.com/en-us/library/ff546850(v=VS.85).aspx
	// * (asterisk) Matches zero or more characters.
	// ? (question mark) Matches a single character.

	// DOS_DOT Matches either a period or zero characters beyond the name string.
	// DOS_QM Matches any single character or, upon encountering a period or end
	//        of name string, advances the expression to the end of the set of
	//        contiguous DOS_QMs.
	// DOS_STAR Matches zero or more characters until encountering and matching
	//          the final . in the name.
	template<class T> bool IsNameInExpressionT(const T* nameStr,
											   const T* expressionStr,
											   bool ignoreCase,
											   const T dotChar,
											   const T starChar,
											   const T questionChar,
											   const T DOS_STAR,
											   const T DOS_QM,
											   const T DOS_DOT
	)
	{
		constexpr const T ZeroChar = T();

		size_t expressionIndex = 0;
		size_t nameIndex = 0;

		if ((!expressionStr || !expressionStr[0]) && (!nameStr || !nameStr[0]))
		{
			return true;
		}

		if (!expressionStr || !nameStr || !expressionStr[0] || !nameStr[0])
		{

			return false;
		}

		while (expressionStr[expressionIndex] && nameStr[nameIndex])
		{

			if (expressionStr[expressionIndex] == starChar)
			{
				expressionIndex++;
				if (expressionStr[expressionIndex] == ZeroChar)
				{
					return true;
				}

				while (nameStr[nameIndex] != ZeroChar)
				{

					if (IsNameInExpressionT(&expressionStr[expressionIndex], &nameStr[nameIndex], ignoreCase, dotChar, starChar, questionChar, DOS_STAR, DOS_QM, DOS_DOT))
					{
						return true;
					}
					nameIndex++;
				}
			}
			else if (expressionStr[expressionIndex] == DOS_STAR)
			{
				size_t position = nameIndex;
				size_t lastDot = 0;
				expressionIndex++;

				while (nameStr[position] != ZeroChar)
				{
					if (nameStr[position] == dotChar)
					{
						lastDot = position;
					}
					position++;
				}

				bool endReached = false;

				while (!endReached)
				{
					endReached = (nameStr[nameIndex] == ZeroChar || nameIndex == lastDot);
					if (!endReached)
					{
						if (IsNameInExpressionT(&expressionStr[expressionIndex], &nameStr[nameIndex], ignoreCase, dotChar, starChar, questionChar, DOS_STAR, DOS_QM, DOS_DOT))
						{
							return true;
						}
						nameIndex++;
					}
				}
			}
			else if (expressionStr[expressionIndex] == DOS_QM)
			{
				expressionIndex++;
				if (nameStr[nameIndex] != dotChar)
				{
					nameIndex++;
				}
				else
				{
					size_t position = nameIndex + 1;
					while (nameStr[position] != ZeroChar)
					{
						if (nameStr[position] == dotChar)
						{
							break;
						}
						position++;
					}

					if (nameStr[position] == dotChar)
					{
						nameIndex++;
					}
				}
			}
			else if (expressionStr[expressionIndex] == DOS_DOT)
			{
				expressionIndex++;
				if (nameStr[nameIndex] == dotChar)
				{
					nameIndex++;
				}
			}
			else
			{
				if (expressionStr[expressionIndex] == questionChar || (ignoreCase && CharToUpper(expressionStr[expressionIndex]) == CharToUpper(nameStr[nameIndex])) || (!ignoreCase && expressionStr[expressionIndex] == nameStr[nameIndex]))
				{
					expressionIndex++;
					nameIndex++;
				}
				else
				{
					return false;
				}
			}
		}

		return !expressionStr[expressionIndex] && !nameStr[nameIndex] ? true : false;
	}

	bool IsNameInExpression(const wchar_t* nameStr, const wchar_t* expressionStr, bool ignoreCase)
	{
		const wchar_t DOS_STAR = L'<';
		const wchar_t DOS_QM = L'>';
		const wchar_t DOS_DOT = L'"';

		const wchar_t dotChar = L'.';
		const wchar_t starChar = L'*';
		const wchar_t questionChar = L'?';

		return IsNameInExpressionT(nameStr, expressionStr, ignoreCase, dotChar, starChar, questionChar, DOS_STAR, DOS_QM, DOS_DOT);
	}
	bool IsNameInExpression(const char* nameStr, const char* expressionStr, bool ignoreCase)
	{
		const char DOS_STAR = L'<';
		const char DOS_QM = L'>';
		const char DOS_DOT = L'"';

		const char dotChar = L'.';
		const char starChar = L'*';
		const char questionChar = L'?';

		return IsNameInExpressionT(nameStr, expressionStr, ignoreCase, dotChar, starChar, questionChar, DOS_STAR, DOS_QM, DOS_DOT);
	}
}

/* Upper/Lower */
char KxString::CharToLower(char c)
{
	return Util::CharToLower(c);
}
wchar_t KxString::CharToLower(wchar_t c)
{
	return Util::CharToLower(c);
}
char KxString::CharToUpper(char c)
{
	return Util::CharToUpper(c);
}
wchar_t KxString::CharToUpper(wchar_t c)
{
	return Util::CharToUpper(c);
}

wxUniChar& KxString::CharMakeLower(wxUniChar& c)
{
	c = Util::CharToLower(static_cast<wchar_t>(c.GetValue()));
	return c;
}
wxUniChar& KxString::CharMakeUpper(wxUniChar& c)
{
	c = Util::CharToUpper(static_cast<wchar_t>(c.GetValue()));
	return c;
}

wxString& KxString::MakeLower(wxString& s)
{
	::CharLowerBuffW(wxStringBuffer(s, s.length()), s.length());
	return s;
}
wxString& KxString::MakeUpper(wxString& s)
{
	::CharUpperBuffW(wxStringBuffer(s, s.length()), s.length());
	return s;
}

wxString& KxString::MakeCapitalized(wxString& s, bool fistCharOnly)
{
	if (!s.IsEmpty())
	{
		if (!fistCharOnly)
		{
			s.MakeLower();
		}
		s[0] = CharToUpper(s[0]);
	}
	return s;
}

/* Match and compare */
KxString::CompareResult KxString::Compare(const std::wstring_view& v1, const std::wstring_view& v2, bool ignoreCase)
{
	return (CompareResult)::CompareStringOrdinal(v1.data(), v1.length(), v2.data(), v2.length(), ignoreCase);
}
KxString::CompareResult KxString::Compare(const wxString& v1, const wxString& v2, bool ignoreCase)
{
	return (CompareResult)::CompareStringOrdinal(v1.wc_str(), v1.length(), v2.wc_str(), v2.length(), ignoreCase);
}

bool KxString::Matches(const std::string_view& string, const std::string_view& mask, bool ignoreCase)
{
	return Util::IsNameInExpression(string.data(), mask.data(), ignoreCase);
}
bool KxString::Matches(const std::wstring_view& string, const std::wstring_view& mask, bool ignoreCase)
{
	return Util::IsNameInExpression(string.data(), mask.data(), ignoreCase);
}
bool KxString::Matches(const wxString& string, const wxString& mask, bool ignoreCase)
{
	return Util::IsNameInExpression(string.wc_str(), mask.wc_str(), ignoreCase);
}

/* Split */
KxStringVector KxString::Split(const wxString& source, size_t partLength, bool allowEmpty)
{
	KxStringVector stringList;

	size_t sourceLength = source.length();
	if (partLength != 0 && partLength < sourceLength && sourceLength != 0)
	{
		stringList.reserve(sourceLength / partLength + 1);

		size_t offset = 0;
		while (offset < sourceLength)
		{
			if (offset + partLength > sourceLength)
			{
				partLength = sourceLength - offset;
			}

			wxString sPart = source.Mid(offset, partLength);
			if (allowEmpty || !sPart.empty())
			{
				stringList.emplace_back(sPart);
			}
			offset += partLength;
		}
	}
	else if (partLength == sourceLength)
	{
		stringList.emplace_back(source);
	}
	return stringList;
}
KxStringVector KxString::Split(const wxString& source, const wxString& pattern, bool allowEmpty)
{
	KxStringVector stringList;
	size_t sourceLength = source.Length();
	size_t patternLength = pattern.Length();

	if (patternLength != 0 && patternLength <= sourceLength && sourceLength != 0)
	{
		size_t offset = 0;
		size_t pos = source.find(pattern, offset);
		while (true)
		{
			wxString part = source.Mid(offset, pos - offset);
			if (allowEmpty || !part.empty())
			{
				stringList.push_back(part);
			}
			if (pos == wxString::npos)
			{
				break;
			}

			offset = pos + patternLength;
			pos = source.find(pattern, offset);
		}
	}
	else if (patternLength == 0)
	{
		stringList.emplace_back(source);
	}
	return stringList;
}

/* Join */
wxString KxString::Join(const KxStringVector& stringList, const wxString& separator)
{
	wxString outString;
	size_t totalLength = 0;
	for (const wxString& s: stringList)
	{
		totalLength += s.size() + separator.size();
	}
	outString.reserve(totalLength);

	size_t i = 0;
	for (const wxString& s: stringList)
	{
		outString += s;
		if (i + 1 != stringList.size())
		{
			outString += separator;
		}
		++i;
	}
	return outString;
}

/* Searching and replacing */
size_t KxString::Find(const wxString& source, const wxString& pattern, size_t startAt, bool caseSensetivity)
{
	size_t pos = wxString::npos;
	if (caseSensetivity)
	{
		if (startAt == wxString::npos)
		{
			pos = source.rfind(pattern, startAt);
		}
		else
		{
			pos = source.find(pattern, startAt);
		}
	}
	else
	{
		wxString sourceL = ToLower(source);
		wxString patternL = ToLower(pattern);
		if (startAt == wxString::npos)
		{
			pos = sourceL.rfind(patternL, startAt);
		}
		else
		{
			pos = sourceL.find(patternL, startAt);
		}
	}
	return pos;
}
size_t KxString::Replace(wxString& source, const wxString& pattern, const wxString& replacement, size_t startAt, bool caseSensetivity)
{
	size_t replacementCount = 0;
	size_t replLength = replacement.length();
	size_t patternLength = pattern.length();
	size_t pos = wxString::npos;

	if (source.IsEmpty() || patternLength == 0)
	{
		return 0;
	}

	wxString sourceL;
	wxString patternL;
	if (caseSensetivity)
	{
		pos = source.find(pattern, startAt);
	}
	else
	{
		sourceL = ToLower(source);
		patternL = ToLower(pattern);

		pos = sourceL.find(patternL, startAt);
	}

	while (pos != wxString::npos)
	{
		source.replace(pos, patternLength, replacement);
		replacementCount++;

		if (caseSensetivity)
		{
			pos = source.find(pattern, pos + replLength);
		}
		else
		{
			pos = sourceL.find(patternL, pos + replLength);
		}
	}
	return replacementCount;
}

wxString KxString::AbbreviateFilePath(const wxString& source, size_t maxChars)
{
	maxChars++;

	wxString result;
	::PathCompactPathExW(wxStringBuffer(result, maxChars), source, maxChars, NULL);
	return result;
}
