/*
Copyright © 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxXDocumentNode.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxString.h"
#include "KxFramework/KxStringUtility.h"
#include <charconv>

namespace
{
	template<class TBuffer, class TValue>
	bool IntToChars(TBuffer&& buffer, TValue value, int base = 10)
	{
		return std::to_chars(std::begin(buffer), std::end(buffer), value, base).ec == std::errc();
	}

	template<class TBuffer, class TValue>
	bool FloatToChars(TBuffer&& buffer, TValue value, int precision)
	{
		return std::to_chars(std::begin(buffer), std::end(buffer), value, std::chars_format::fixed, precision).ec == std::errc();
	}
}

std::pair<wxStringView, int> KxIXDocumentNode::ExtractIndexFromName(wxStringView elementName, wxStringView xPathSeparator)
{
	int index = 0;
	if (!xPathSeparator.empty())
	{
		size_t indexStart = elementName.find(xPathSeparator);
		if (indexStart != wxString::npos)
		{
			long value = 0;
			if (KxUtility::String::FromStringView(elementName.substr(indexStart + xPathSeparator.length())).ToCLong(&value))
			{
				index = std::clamp<int>(value, 0, std::numeric_limits<int>::max());
				elementName = elementName.substr(0, indexStart);
			}
		}
	}
	return {elementName, index};
}
bool KxIXDocumentNode::ContainsForbiddenCharactersForValue(const wxString& value)
{
	for (wxChar c: value)
	{
		if (c == wxS('&') || c == wxS('<') || c == wxS('>'))
		{
			return true;
		}
	}
	return false;
}

wxString KxIXDocumentNode::FormatInt(int64_t value, int base) const
{
	char buffer[32] = {0};
	if (IntToChars(buffer, value, base))
	{
		return buffer;
	}
	return wxEmptyString;
}
wxString KxIXDocumentNode::FormatPointer(const void* value) const
{
	char buffer[32] = {0};
	if (IntToChars(buffer, reinterpret_cast<size_t>(value), 16))
	{
		return KxString::Format(wxS("0x%1"), buffer);
	}
	return wxEmptyString;
}
wxString KxIXDocumentNode::FormatFloat(double value, int precision) const
{
	// TODO: replace by inactive code below on MSVC update
	return wxString::FromCDouble(value, precision);
	#if 0
	char buffer[32] = {0};
	if (FloatToChars(buffer, value, precision))
	{
		return buffer;
	}
	return wxEmptyString;
	#endif
}
wxString KxIXDocumentNode::FormatBool(bool value) const
{
	return value ? wxS("1") : wxS("0");
}

int64_t KxIXDocumentNode::ParseInt(const wxString& value, int base, int64_t defaultValue) const
{
	long long iValue = defaultValue;
	if (value.ToLongLong(&iValue, base))
	{
		return iValue;
	}
	return defaultValue;
}
void* KxIXDocumentNode::ParsePointer(const wxString& value, void* defaultValue) const
{
	wxString intValue;
	if (value.StartsWith(wxS("0x"), &intValue))
	{
		return reinterpret_cast<void*>(ParseInt(intValue, 16, reinterpret_cast<int64_t>(defaultValue)));
	}
	return defaultValue;
}
double KxIXDocumentNode::ParseFloat(const wxString& value, double defaultValue) const
{
	double dValue = defaultValue;
	if (value.ToCDouble(&dValue))
	{
		return dValue;
	}
	return defaultValue;
}
bool KxIXDocumentNode::ParseBool(const wxString& value, bool defaultValue) const
{
	bool isUnknown = false;
	bool result = KxUtility::StringToBool(value, &isUnknown);
	return isUnknown ? defaultValue : result;
}
