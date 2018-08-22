/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxFormat.h"
#include "KxFramework/KxString.h"

namespace Utils
{
	template<class T> wxString FormatIntWithBase(T value, int base = 10, bool upper = false)
	{
		static const wxChar* digitsL = wxS("0123456789abcdefghijklmnopqrstuvwxyz");
		static const wxChar* digitsU = wxS("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
		const wxChar* digits = upper ? digitsU : digitsL;

		wxString result;
		if (base >= 2 && base <= 36)
		{
			do
			{
				result = digits[value % base] + result;
				value /= base;
			}
			while (value);

			if constexpr(std::is_signed<T>::value)
			{
				if (value < 0)
				{
					result = wxS('-') + result;
				}
			}
		}
		return result;
	};
}

void KxFormat::FindAndReplace(const wxString& string, size_t index, size_t startAt)
{
	wchar_t indexBuffer[64] = {0};
	swprintf_s(indexBuffer, L"%%%zu", index);

	bool ok = false;
	size_t count = 0;
	do
	{
		ok = FindAndReplace(string, indexBuffer, startAt);
		count++;
	}
	while (ok && count < 1024);
}
bool KxFormat::FindAndReplace(const wxString& string, const std::wstring_view& index, size_t startAt)
{
	size_t pos = m_String.find(index.data(), startAt);
	if (pos != wxString::npos && m_String.length() > pos + 1)
	{
		m_String.replace(pos, index.length(), string);
		return true;
	}
	return false;
}
void KxFormat::FindCurrentAndReplace(const wxString& string)
{
	FindAndReplace(string, m_CurrentArgument);
	m_CurrentArgument++;
}

KxFormat& KxFormat::arg(const wxString& a, int fieldWidth, const wxUniChar& fillChar)
{
	if (fieldWidth == 0 || a.length() >= (size_t)std::abs(fieldWidth))
	{
		FindCurrentAndReplace(a);
	}
	else if (fieldWidth > 0)
	{
		wxString copy(fillChar, (size_t)fieldWidth - a.length());
		copy += a;
		FindCurrentAndReplace(copy);
	}
	else if (fieldWidth < 0)
	{
		wxString copy(a);
		copy.append(-fieldWidth - a.length(), fillChar);
		FindCurrentAndReplace(copy);
	}
	return *this;
}

KxFormat& KxFormat::arg(int8_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return arg(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}
KxFormat& KxFormat::arg(uint8_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return arg(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}

KxFormat& KxFormat::arg(int16_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return arg(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}
KxFormat& KxFormat::arg(uint16_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return arg(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}

KxFormat& KxFormat::arg(uint32_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return arg(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}
KxFormat& KxFormat::arg(int32_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return arg(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}

KxFormat& KxFormat::arg(int64_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return arg(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}
KxFormat& KxFormat::arg(uint64_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return arg(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}

KxFormat& KxFormat::arg(double a, const wxUniChar& format, int precision, int fieldWidth, const wxUniChar& fillChar)
{
	switch (format.GetValue())
	{
		case L'g':
		case L'G':
		case L'e':
		case L'E':
		case L'f':
		{
			break;
		}
		default:
		{
			return *this;
		}
	};

	wchar_t formatString[64] = {0};
	if (precision >= 0)
	{
		swprintf_s(formatString, L"%%.%d%c", precision, (wchar_t)format);
	}
	else
	{
		swprintf_s(formatString, L"%%%c", (wchar_t)format);
	}
	return arg(wxString::Format(formatString, a), fieldWidth, fieldWidth);
}
