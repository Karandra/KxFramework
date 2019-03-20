/*
Copyright © 2018-2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxFormat.h"
#include "KxFramework/KxString.h"

void KxFormatBase::FindAndReplace(const wxString& string, size_t index, size_t startAt)
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
bool KxFormatBase::FindAndReplace(const wxString& string, const std::wstring_view& index, size_t startAt)
{
	size_t pos = m_String.find(index.data(), startAt);
	if (pos != wxString::npos && m_String.length() > pos + 1)
	{
		m_String.replace(pos, index.length(), string);
		return true;
	}
	return false;
}
void KxFormatBase::FindCurrentAndReplace(const wxString& string)
{
	FindAndReplace(string, m_CurrentArgument);
	m_CurrentArgument++;
}

void KxFormatBase::FormatString(const wxString& arg, int fieldWidth, wxUniChar fillChar)
{
	if (fieldWidth == 0 || arg.length() >= (size_t)std::abs(fieldWidth))
	{
		FindCurrentAndReplace(arg);
	}
	else if (fieldWidth > 0)
	{
		wxString copy(fillChar, (size_t)fieldWidth - arg.length());
		copy += arg;
		FindCurrentAndReplace(copy);
	}
	else if (fieldWidth < 0)
	{
		wxString copy(arg);
		copy.append(static_cast<size_t>(-fieldWidth) - arg.length(), fillChar);
		FindCurrentAndReplace(copy);
	}
}
void KxFormatBase::FormatChar(wxUniChar arg, int fieldWidth, wxUniChar fillChar)
{
	FormatString(wxString(arg), fieldWidth, fillChar);
}

void KxFormatBase::FormatPointer(const void* arg, int fieldWidth, wxUniChar fillChar)
{
	const size_t value = reinterpret_cast<size_t>(const_cast<void*>(arg));
	FormatString(FormatIntWithBase(value, 16, m_IsUpperCase), fieldWidth, fillChar);
}
void KxFormatBase::FormatBool(bool arg, int fieldWidth, wxUniChar fillChar)
{
	static const wxChar ms_TrueU[] = wxS("TRUE");
	static const wxChar ms_FalseU[] = wxS("FALSE");
	
	static const wxChar ms_TrueL[] = wxS("true");
	static const wxChar ms_FalseL[] = wxS("false");

	if (m_IsUpperCase)
	{
		FormatString(arg ? ms_TrueU : ms_FalseU, fieldWidth, fillChar);
	}
	else
	{
		FormatString(arg ? ms_TrueL : ms_FalseL, fieldWidth, fillChar);
	}
}
void KxFormatBase::FormatDouble(double arg, int precision, int fieldWidth, wxUniChar format, wxUniChar fillChar)
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
			return;
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
	FormatString(wxString::Format(formatString, arg), fieldWidth, fieldWidth);
}

#if 0
static void TestFunction()
{
	KxFormat format("%1 %2 %3");

	format(true);
	format(false);

	wxString strWx;
	std::string strStd;
	std::wstring strStdW;
	std::string_view strStdV;
	std::wstring_view strStdWV;

	format(strWx);
	format(strStd);
	format(strStdW);
	format(strStdV);
	format(strStdWV);

	format("a");
	format(L"a");
	format('a');
	format(L'a');

	format(wxUniChar('a'));
	format(strWx[0]);

	format(1);
	format(1u);

	format(1i8);
	format(1ui8);

	format(1i16);
	format(1ui16);

	format(1i32);
	format(1ui32);

	format(1i64);
	format(1ui64);

	void* p1 = nullptr;
	const void* p2 = nullptr;
	format(p1);
	format(p2);

	int* pi1 = nullptr;
	const int* pi2 = nullptr;
	format(pi1);
	format(pi2);

	const char* c1 = nullptr;
	char* c2 = nullptr;
	format(c1);
	format(c2);

	const wchar_t* wc1 = nullptr;
	wchar_t* wc2 = nullptr;
	format(wc1);
	format(wc2);

	format(7.0);
	format(2.7f);

	enum e1
	{
		e1_value,
	};
	enum class e2
	{
		e2_value,
	};

	format(e1::e1_value);
	format(e2::e2_value);

	struct Integer
	{
		int a = 42;

		operator int() const
		{
			return a;
		}
	};
	//format(Integer());
}
#endif
