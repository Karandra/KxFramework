#include "KxStdAfx.h"
#include "String.h"
#include "Kx/Utility/Common.h"

namespace
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
		return reinterpret_cast<char>(::CharUpperA(reinterpret_cast<LPSTR>(c)));
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
		return reinterpret_cast<wchar_t>(::CharUpperW(reinterpret_cast<LPWSTR>(c)));
	}

	void StringMakeLower(wxString& string) noexcept
	{
		::CharLowerBuffW(wxStringBuffer(string, string.length()), string.length());
	}
	void StringMakeUpper(wxString& string) noexcept
	{
		::CharUpperBuffW(wxStringBuffer(string, string.length()), string.length());
	}
	wxString StringToLower(const wxScopedCharTypeBuffer<wxChar> string) noexcept
	{
		wxString result = string;
		StringMakeLower(result);
		return result;
	}
	wxString StringToUpper(const wxScopedCharTypeBuffer<wxChar> string) noexcept
	{
		wxString result = string;
		StringMakeUpper(result);
		return result;
	}

	int CompareStrings(std::string_view left, std::string_view right, bool ignoreCase) noexcept
	{
		if (ignoreCase)
		{
			return KxFramework::String(left).MakeLower() == KxFramework::String(right).MakeLower();
		}
		else
		{
			return left.compare(right);
		}
	}
	int CompareStrings(std::wstring_view left, std::wstring_view right, bool ignoreCase) noexcept
	{
		if (ignoreCase)
		{
			constexpr size_t maxLength = std::numeric_limits<int>::max();
			wxASSERT_MSG(left.length() <= maxLength && right.length() <= maxLength, __FUNCTION__ ": strings are too long to be compared using 'CompareStringOrdinal'");

			switch (::CompareStringOrdinal(left.data(), left.length(), right.data(), right.length(), ignoreCase))
			{
				case CSTR_LESS_THAN:
				{
					return -1;
				}
				case CSTR_EQUAL:
				{
					return 0;
				}
				case CSTR_GREATER_THAN:
				{
					return 1;
				}
			};
			return -2;
		}
		else
		{
			return left.compare(right);
		}
	}
	int CompareChars(wxUniChar left, wxUniChar right) noexcept
	{
		if (left < right)
		{
			return -1;
		}
		else if (left > right)
		{
			return 1;
		}
		return 0;
	}

	template<class T>
	bool IsNameInExpressionImpl(std::basic_string_view<T> name,
								std::basic_string_view<T> expression,
								bool ignoreCase,
								const T dotChar,
								const T starChar,
								const T questionChar,
								const T DOS_STAR,
								const T DOS_QM,
								const T DOS_DOT
	)
	{
		// Check whether Name matches Expression
		// Expression can contain "?"(any one character) and "*" (any string)
		// when IgnoreCase is true, do case insensitive matching
		//
		// http://msdn.microsoft.com/en-us/library/ff546850(v=VS.85).aspx
		// * (asterisk) Matches zero or more characters.
		// ? (question mark) Matches a single character.
		//
		// DOS_DOT Matches either a period or zero characters beyond the name string.
		// DOS_QM Matches any single character or, upon encountering a period or end
		//        of name string, advances the expression to the end of the set of
		//        contiguous DOS_QMs.
		// DOS_STAR Matches zero or more characters until encountering and matching
		//          the final . in the name.

		// Both name and expression are empty, return true.
		if (name.empty() && expression.empty())
		{
			return true;
		}

		// Expression is empty or it's star char ('*'), return true.
		if (expression.empty() || (expression.length() == 1 && expression[0] == starChar))
		{
			return true;
		}

		// Empty name or empty expression, return false.
		if (name.empty() || expression.empty())
		{
			return false;
		}

		size_t nameIndex = 0;
		size_t expressionIndex = 0;
		while (expressionIndex < expression.length() && nameIndex < name.length())
		{
			if (expression[expressionIndex] == starChar)
			{
				expressionIndex++;
				if (expressionIndex >= expression.length())
				{
					return true;
				}

				while (nameIndex < name.length())
				{
					if (IsNameInExpressionImpl(expression.substr(expressionIndex), name.substr(nameIndex), ignoreCase, dotChar, starChar, questionChar, DOS_STAR, DOS_QM, DOS_DOT))
					{
						return true;
					}
					nameIndex++;
				}
			}
			else if (expression[expressionIndex] == DOS_STAR)
			{
				expressionIndex++;
				size_t lastDot = 0;

				size_t position = nameIndex;
				while (position < name.length())
				{
					if (name[position] == dotChar)
					{
						lastDot = position;
					}
					position++;
				}

				bool endReached = false;
				do
				{
					endReached = nameIndex >= name.length() || nameIndex == lastDot;
					if (!endReached)
					{
						if (IsNameInExpressionImpl(expression.substr(expressionIndex), name.substr(nameIndex), ignoreCase, dotChar, starChar, questionChar, DOS_STAR, DOS_QM, DOS_DOT))
						{
							return true;
						}
						nameIndex++;
					}
				}
				while (!endReached);
			}
			else if (expression[expressionIndex] == DOS_QM)
			{
				expressionIndex++;
				if (name[nameIndex] != dotChar)
				{
					nameIndex++;
				}
				else
				{
					size_t position = nameIndex + 1;
					while (position < name.length())
					{
						if (name[position] == dotChar)
						{
							break;
						}
						position++;
					}

					if (name[position] == dotChar)
					{
						nameIndex++;
					}
				}
			}
			else if (expression[expressionIndex] == DOS_DOT)
			{
				expressionIndex++;
				if (name[nameIndex] == dotChar)
				{
					nameIndex++;
				}
			}
			else
			{
				if (expression[expressionIndex] == questionChar || (ignoreCase && CharToUpper(expression[expressionIndex]) == CharToUpper(name[nameIndex])) || (!ignoreCase && expression[expressionIndex] == name[nameIndex]))
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
		return !expression[expressionIndex] && !name[nameIndex] ? true : false;
	}
	bool IsNameInExpression(std::string_view name, std::string_view expression, bool ignoreCase)
	{
		constexpr char DOS_STAR = L'<';
		constexpr char DOS_QM = L'>';
		constexpr char DOS_DOT = L'"';

		constexpr char dotChar = L'.';
		constexpr char starChar = L'*';
		constexpr char questionChar = L'?';

		return IsNameInExpressionImpl(name, expression, ignoreCase, dotChar, starChar, questionChar, DOS_STAR, DOS_QM, DOS_DOT);
	}
	bool IsNameInExpression(std::wstring_view name, std::wstring_view expression, bool ignoreCase)
	{
		constexpr wchar_t DOS_STAR = L'<';
		constexpr wchar_t DOS_QM = L'>';
		constexpr wchar_t DOS_DOT = L'"';

		constexpr wchar_t dotChar = L'.';
		constexpr wchar_t starChar = L'*';
		constexpr wchar_t questionChar = L'?';

		return IsNameInExpressionImpl(name, expression, ignoreCase, dotChar, starChar, questionChar, DOS_STAR, DOS_QM, DOS_DOT);
	}
}

namespace KxFramework
{
	const String NullString;
}

namespace KxFramework
{
	int String::Compare(std::string_view left, std::string_view right, StringOpFlag flags) noexcept
	{
		return CompareStrings(left, right, flags & StringOpFlag::IgnoreCase);
	}
	int String::Compare(std::wstring_view left, std::wstring_view right, StringOpFlag flags) noexcept
	{
		return CompareStrings(left, right, flags & StringOpFlag::IgnoreCase);
	}
	int String::Compare(wxUniChar left, wxUniChar right, StringOpFlag flags) noexcept
	{
		if (flags & StringOpFlag::IgnoreCase)
		{
			return CompareChars(ToLower(left), ToLower(right));
		}
		else
		{
			return CompareChars(left, right);
		}
	}

	bool String::Matches(std::string_view name, std::string_view expression, StringOpFlag flags) noexcept
	{
		return IsNameInExpression(name, expression, flags & StringOpFlag::IgnoreCase);
	}
	bool String::Matches(std::wstring_view name, std::wstring_view expression, StringOpFlag flags) noexcept
	{
		return IsNameInExpression(name, expression, flags & StringOpFlag::IgnoreCase);
	}

	wxUniChar String::ToLower(wxUniChar c) noexcept
	{
		return CharToUpper(static_cast<wchar_t>(c));
	}
	wxUniChar String::ToUpper(wxUniChar c) noexcept
	{
		return CharToLower(static_cast<wchar_t>(c));
	}

	// Comparison
	bool String::StartsWith(std::string_view pattern, String* rest, StringOpFlag flags) const
	{
		String patternCopy = FromView(pattern);
		return StartsWith(patternCopy, rest, flags);
	}
	bool String::StartsWith(std::wstring_view pattern, String* rest, StringOpFlag flags) const
	{
		if (pattern.empty())
		{
			return false;
		}

		Utility::RemoveFlag(flags, StringOpFlag::FromEnd);
		const size_t pos = Find(pattern, 0, flags);
		if (pos == 0)
		{
			if (rest)
			{
				*rest = m_String.substr(pos, pattern.length());
			}
			return true;
		}
		return false;
	}
	
	bool String::EndsWith(std::string_view pattern, String* rest, StringOpFlag flags) const
	{
		String patternCopy = FromView(pattern);
		return EndsWith(patternCopy, rest, flags);
	}
	bool String::EndsWith(std::wstring_view pattern, String* rest, StringOpFlag flags) const
	{
		if (pattern.empty())
		{
			return false;
		}

		const size_t pos = Find(pattern, 0, flags|StringOpFlag::FromEnd);
		if (pos == m_String.length() - pattern.length())
		{
			if (rest)
			{
				*rest = m_String.substr(pos, npos);
			}
			return true;
		}
		return false;
	}

	// Substring extraction
	String String::AfterFirst(wxUniChar c, String* rest, StringOpFlag flags) const
	{
		const size_t pos = Find(c, 0, flags);
		if (pos != npos)
		{
			if (rest)
			{
				*rest = SubString(0, pos);
			}
			return Mid(pos + 1);
		}
		return {};
	}
	String String::AfterLast(wxUniChar c, String* rest, StringOpFlag flags) const
	{
		const size_t pos = Find(c, 0, flags|StringOpFlag::FromEnd);
		if (pos != npos)
		{
			if (rest)
			{
				*rest = SubString(0, pos);
			}
			return Mid(pos + 1);
		}
		return {};
	}

	String String::BeforeFirst(wxUniChar c, String* rest, StringOpFlag flags) const
	{
		const size_t pos = Find(c, 0, flags);
		if (pos != npos)
		{
			if (rest)
			{
				*rest = Mid(pos + 1);
			}
			return SubString(0, pos);
		}
		return {};
	}
	String String::BeforeLast(wxUniChar c, String* rest, StringOpFlag flags) const
	{
		const size_t pos = Find(c, 0, flags|StringOpFlag::FromEnd);
		if (pos != npos)
		{
			if (rest)
			{
				*rest = Mid(pos + 1);
			}
			return SubString(0, pos);
		}
		return {};
	}

	// Case conversion
	String& String::MakeLower() noexcept
	{
		StringMakeLower(m_String);
		return *this;
	}
	String& String::MakeUpper() noexcept
	{
		StringMakeUpper(m_String);
		return *this;
	}

	// Searching and replacing
	size_t String::DoFind(std::string_view pattern, size_t offset, StringOpFlag flags) const
	{
		String patternCopy = FromView(pattern);
		return Find(StringViewOf(patternCopy), offset, flags);
	}
	size_t String::DoFind(std::wstring_view pattern, size_t offset, StringOpFlag flags) const
	{
		if (!m_String.IsEmpty() && offset < m_String.length())
		{
			if (flags & StringOpFlag::IgnoreCase)
			{
				wxString sourceL = StringToLower(ScopedCharBufferOf(m_String));
				wxString patternL = StringToLower(ScopedCharBufferOf(pattern));

				if (flags & StringOpFlag::FromEnd)
				{
					return sourceL.rfind(patternL, offset);
				}
				else
				{
					return sourceL.find(patternL, offset);
				}
			}
			else
			{
				if (flags & StringOpFlag::FromEnd)
				{
					return m_String.rfind(ScopedCharBufferOf(pattern), offset);
				}
				else
				{
					return m_String.find(ScopedCharBufferOf(pattern), offset);
				}
			}
		}
		return npos;
	}
	size_t String::DoFind(wxUniChar pattern, size_t offset, StringOpFlag flags) const noexcept
	{
		if (m_String.IsEmpty() || offset >= m_String.length())
		{
			return npos;
		}

		if (flags & StringOpFlag::FromEnd)
		{
			for (size_t i = m_String.length() - 1 - offset; i != 0; i--)
			{
				if (Compare(m_String[i], pattern, flags) == 0)
				{
					return i;
				}
			}
		}
		else
		{
			for (size_t i = offset; i < m_String.length(); i++)
			{
				if (Compare(m_String[i], pattern, flags) == 0)
				{
					return i;
				}
			}
		}
		return npos;
	}

	size_t String::DoReplace(std::string_view pattern, std::string_view replacement, size_t offset, StringOpFlag flags)
	{
		String patternCopy = FromView(pattern);
		String replacementCopy = FromView(replacement);
		return Replace(StringViewOf(patternCopy), StringViewOf(replacementCopy), offset, flags);
	}
	size_t String::DoReplace(std::wstring_view pattern, std::wstring_view replacement, size_t offset, StringOpFlag flags)
	{
		const size_t replacementLength = replacement.length();
		const size_t patternLength = pattern.length();

		if (m_String.IsEmpty() || patternLength == 0 || offset >= m_String.length())
		{
			return 0;
		}

		size_t replacementCount = 0;
		size_t pos = wxString::npos;

		wxString sourceL;
		wxString patternL;
		if (flags & StringOpFlag::IgnoreCase)
		{
			if (flags & StringOpFlag::FromEnd)
			{
				pos = m_String.rfind(ScopedCharBufferOf(pattern), offset);
			}
			else
			{
				pos = m_String.find(ScopedCharBufferOf(pattern), offset);
			}
		}
		else
		{
			Private::MoveWxString(patternL, StringToLower(ScopedCharBufferOf(m_String)));
			Private::MoveWxString(patternL, StringToLower(ScopedCharBufferOf(pattern)));

			if (flags & StringOpFlag::FromEnd)
			{
				pos = sourceL.rfind(patternL, offset);
			}
			else
			{
				pos = sourceL.find(patternL, offset);
			}
		}

		while (pos != wxString::npos)
		{
			m_String.replace(pos, patternLength, replacement.data(), replacement.length());
			replacementCount++;

			if (flags & StringOpFlag::FirstMatchOnly)
			{
				return replacementCount;
			}

			if (flags & StringOpFlag::IgnoreCase)
			{
				if (flags & StringOpFlag::FromEnd)
				{
					pos = m_String.rfind(ScopedCharBufferOf(pattern), pos + replacementLength);
				}
				else
				{
					pos = m_String.find(ScopedCharBufferOf(pattern), pos + replacementLength);
				}
			}
			else
			{
				if (flags & StringOpFlag::FromEnd)
				{
					pos = sourceL.rfind(patternL, pos + replacementLength);
				}
				else
				{
					pos = sourceL.find(patternL, pos + replacementLength);
				}
			}
		}
		return replacementCount;
	}
	size_t String::DoReplace(wxUniChar pattern, wxUniChar replacement, size_t offset, StringOpFlag flags) noexcept
	{
		if (m_String.IsEmpty() || offset >= m_String.length())
		{
			return 0;
		}

		size_t replacementCount = 0;
		auto TestAndReplace = [&](wxUniCharRef c)
		{
			if (Compare(c, pattern, flags) == 0)
			{
				c = replacement;
				replacementCount++;

				return !(flags & StringOpFlag::FirstMatchOnly);
			}
			return false;
		};

		if (flags & StringOpFlag::FromEnd)
		{
			for (size_t i = m_String.length() - 1 - offset; i != 0; i--)
			{
				if (!TestAndReplace(m_String[i]))
				{
					return replacementCount;
				}
			}
		}
		else
		{
			for (size_t i = offset; i < m_String.length(); i++)
			{
				if (!TestAndReplace(m_String[i]))
				{
					return replacementCount;
				}
			}
		}
		return replacementCount;
	}
}
