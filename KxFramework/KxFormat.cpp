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

KxFormat& KxFormat::argString(const wxString& a, int fieldWidth, const wxUniChar& fillChar)
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
KxFormat& KxFormat::argChar(const wxUniChar& a, int fieldWidth, const wxUniChar& fillChar)
{
	return argString(wxString(a), fieldWidth, fillChar);
}

KxFormat& KxFormat::argInt(int8_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return argString(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}
KxFormat& KxFormat::argInt(uint8_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return argString(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}

KxFormat& KxFormat::argInt(int16_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return argString(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}
KxFormat& KxFormat::argInt(uint16_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return argString(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}

KxFormat& KxFormat::argInt(uint32_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return argString(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}
KxFormat& KxFormat::argInt(int32_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return argString(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}

KxFormat& KxFormat::argInt(int64_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return argString(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}
KxFormat& KxFormat::argInt(uint64_t a, int fieldWidth, int base, const wxUniChar& fillChar)
{
	return argString(Utils::FormatIntWithBase(a, base, m_UpperCase), fieldWidth, fillChar);
}

KxFormat& KxFormat::argPointer(const void* a, bool add0x, int fieldWidth, const wxUniChar& fillChar)
{
	const size_t value = reinterpret_cast<size_t>(const_cast<void*>(a));
	if (add0x)
	{
		return argString(Utils::FormatIntWithBase(value, 16, m_UpperCase), fieldWidth, fillChar);
	}
	else
	{
		return argString(wxS("0x") + Utils::FormatIntWithBase(value, 16, m_UpperCase), fieldWidth, fillChar);
	}
}
KxFormat& KxFormat::argBool(bool a, int fieldWidth, const wxUniChar& fillChar)
{
	static const wxChar* ms_TrueU = wxS("TRUE");
	static const wxChar* ms_FalseU = wxS("FALSE");
	
	static const wxChar* ms_TrueL = wxS("true");
	static const wxChar* ms_FalseL = wxS("false");

	if (m_UpperCase)
	{
		return argString(a ? ms_TrueU : ms_FalseU, fieldWidth, fillChar);
	}
	else
	{
		return argString(a ? ms_TrueL : ms_FalseL, fieldWidth, fillChar);
	}
}

KxFormat& KxFormat::argDouble(double a, int precision, int fieldWidth, const wxUniChar& format, const wxUniChar& fillChar)
{
	switch (format.GetValue())
	{
		case L'g':
		case L'G':
		case L'e':
		case L'E':
		case L'f':
		case L'F':
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
	return argString(wxString::Format(formatString, a), fieldWidth, fieldWidth);
}

#if 0
void TestFunction()
{
	KxFormat format("%1 %2 %3");

	wxString strWx;
	std::string strStd;
	std::wstring strStdW;
	std::string_view strStdV;
	std::wstring_view strStdWV;

	format.arg(strWx);
	format.arg(strStd);
	format.arg(strStdW);
	format.arg(strStdV);
	format.arg(strStdWV);

	format.arg("a");
	format.arg(L"a");
	format.arg('a');
	format.arg(L'a');

	format.arg(wxUniChar('a'));
	format.arg(strWx[0]);

	format.arg(1);
	format.arg(1u);

	format.arg(1i8);
	format.arg(1ui8);

	format.arg(1i16);
	format.arg(1ui16);

	format.arg(1i32);
	format.arg(1ui32);

	format.arg(1i64);
	format.arg(1ui64);

	void* p1 = NULL;
	const void* p2 = NULL;
	format.arg(p1);
	format.arg(p2);

	const char* c1 = NULL;
	char* c2 = NULL;
	format.arg(c1);
	format.arg(c2);

	const wchar_t* wc1 = NULL;
	wchar_t* wc2 = NULL;
	format.arg(wc1);
	format.arg(wc2);

	format.arg(7.0);
	format.arg(2.7f);

	enum e1
	{
		e1_value,
	};
	enum class e2
	{
		e2_value,
	};

	format.arg(e1::e1_value);
	format.arg(e2::e2_value);
}
#endif
