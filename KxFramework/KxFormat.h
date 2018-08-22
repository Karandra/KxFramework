/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"

class KxFormat
{
	private:
		wxString m_String;
		size_t m_CurrentArgument = 1;
		bool m_UpperCase = false;

	private:
		bool FindAndReplace(const wxString& string, const std::wstring_view& index, size_t startAt = 0);
		void FindAndReplace(const wxString& string, size_t index, size_t startAt = 0);
		void FindCurrentAndReplace(const wxString& string);

	public:
		KxFormat(const wxString& format)
			:m_String(format)
		{
		}

	public:
		operator wxString() const
		{
			return m_String;
		}

		// Switches
		KxFormat& UpperCase(bool value = true)
		{
			m_UpperCase = value;
			return *this;
		}
		KxFormat& LowerCase(bool value = true)
		{
			m_UpperCase = value;
			return *this;
		}

	public:
		// Strings
		KxFormat& arg(const wxString& a, int fieldWidth = 0, const wxUniChar& fillChar = ' ');
		KxFormat& arg(const wxUniChar& a, int fieldWidth = 0, const wxUniChar& fillChar = ' ')
		{
			return arg(wxString(a), fieldWidth, fillChar);
		}
		KxFormat& arg(char a, int fieldWidth = 0, const wxUniChar& fillChar = ' ')
		{
			return arg(wxUniChar(a), fieldWidth, fillChar);
		}
		KxFormat& arg(wchar_t a, int fieldWidth = 0, const wxUniChar& fillChar = ' ')
		{
			return arg(wxUniChar(a), fieldWidth, fillChar);
		}

		// Integers
		KxFormat& arg(int8_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = ' ');
		KxFormat& arg(uint8_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = ' ');

		KxFormat& arg(int16_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = ' ');
		KxFormat& arg(uint16_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = ' ');

		KxFormat& arg(int32_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = ' ');
		KxFormat& arg(uint32_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = ' ');

		KxFormat& arg(int64_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = ' ');
		KxFormat& arg(uint64_t a, int fieldWidth = 0, int base = 10, const wxUniChar& fillChar = ' ');

		// Floats
		KxFormat& arg(double a, const wxUniChar& format = 'g', int precision = -1, int fieldWidth = 0, const wxUniChar& fillChar = ' ');
};
