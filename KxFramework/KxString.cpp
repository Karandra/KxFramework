/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxString.h"
#include <Shlwapi.h>
#include "KxWinUndef.h"

/* Upper/Lower */
wxUniChar& KxString::MakeLower(wxUniChar& c)
{
	#pragma warning(suppress: 4312)
	#pragma warning(suppress: 4302)
	c = reinterpret_cast<wchar_t>(::CharLowerW(reinterpret_cast<LPWSTR>(c.GetValue())));
	return c;
}
wxUniChar& KxString::MakeUpper(wxUniChar& c)
{
	#pragma warning(suppress: 4312)
	#pragma warning(suppress: 4302)
	c = reinterpret_cast<wchar_t>(::CharUpperW(reinterpret_cast<LPWSTR>(c.GetValue())));
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
		s[0] = ToUpper(s[0]);
	}
	return s;
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
