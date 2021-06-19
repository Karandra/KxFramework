#include "KxfPCH.h"
#include "String.h"
#include "IEncodingConverter.h"
#include "kxf/IO/IStream.h"
#include "kxf/Utility/Common.h"
#include <kxf/System/UndefWindows.h>
#include <wx/string.h>
#include <cctype>

namespace
{
	char DoCharToLower(char c) noexcept
	{
		#pragma warning(suppress: 4312)
		#pragma warning(suppress: 4302)
		return reinterpret_cast<char>(::CharLowerA(reinterpret_cast<LPSTR>(c)));
	}
	char DoCharToUpper(char c) noexcept
	{
		#pragma warning(suppress: 4312)
		#pragma warning(suppress: 4302)
		return reinterpret_cast<char>(::CharUpperA(reinterpret_cast<LPSTR>(c)));
	}
	wchar_t DoCharToLower(wchar_t c) noexcept
	{
		#pragma warning(suppress: 4312)
		#pragma warning(suppress: 4302)
		return reinterpret_cast<wchar_t>(::CharLowerW(reinterpret_cast<LPWSTR>(c)));
	}
	wchar_t DoCharToUpper(wchar_t c) noexcept
	{
		#pragma warning(suppress: 4312)
		#pragma warning(suppress: 4302)
		return reinterpret_cast<wchar_t>(::CharUpperW(reinterpret_cast<LPWSTR>(c)));
	}

	void StringMakeLower(std::wstring& string) noexcept
	{
		::CharLowerBuffW(string.data(), string.length());
	}
	void StringMakeUpper(std::wstring& string) noexcept
	{
		::CharUpperBuffW(string.data(), string.length());
	}
	void StringMakeLower(wxString& string) noexcept
	{
		::CharLowerBuffW(wxStringBuffer(string, string.length()), string.length());
	}
	void StringMakeUpper(wxString& string) noexcept
	{
		::CharUpperBuffW(wxStringBuffer(string, string.length()), string.length());
	}

	wxString StringToLower(const wxScopedCharTypeBuffer<wxChar>& string) noexcept
	{
		wxString result = string;
		StringMakeLower(result);
		return result;
	}
	wxString StringToUpper(const wxScopedCharTypeBuffer<wxChar>& string) noexcept
	{
		wxString result = string;
		StringMakeUpper(result);
		return result;
	}

	std::strong_ordering DoCompareStrings(std::string_view left, std::string_view right, bool ignoreCase) noexcept
	{
		if (ignoreCase)
		{
			return kxf::String(left).MakeLower().CompareTo(kxf::String(right).MakeLower());
		}
		else
		{
			return left <=> right;
		}
	}
	std::strong_ordering DoCompareStrings(std::wstring_view left, std::wstring_view right, bool ignoreCase) noexcept
	{
		if (ignoreCase)
		{
			constexpr size_t maxLength = std::numeric_limits<int>::max();
			wxASSERT_MSG(left.length() <= maxLength && right.length() <= maxLength, __FUNCTION__ ": strings are too long to be compared using 'CompareStringOrdinal'");

			switch (::CompareStringOrdinal(left.data(), left.length(), right.data(), right.length(), ignoreCase))
			{
				case CSTR_LESS_THAN:
				{
					return std::strong_ordering::less;
				}
				case CSTR_EQUAL:
				{
					return std::strong_ordering::equal;
				}
				case CSTR_GREATER_THAN:
				{
					return std::strong_ordering::greater;
				}
			};

			// We shouldn't be here, but in case we did compare with case
			return left <=> right;
		}
		else
		{
			return left <=> right;
		}
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
				if (expression[expressionIndex] == questionChar || (ignoreCase && DoCharToUpper(expression[expressionIndex]) == DoCharToUpper(name[nameIndex])) || (!ignoreCase && expression[expressionIndex] == name[nameIndex]))
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

	template<class T, class TFunc>
	bool ConvertToInteger(T& value, int base, const kxf::XChar* start, TFunc&& func) noexcept
	{
		errno = 0;
		kxf::XChar* end = nullptr;
		auto result = std::invoke(func, start, &end, base);

		if (end != start && errno == 0)
		{
			value = result;
			return true;
		}
		return false;
	}

	template<class T, class TFunc>
	bool ConvertToFloat(T& value, const kxf::XChar* start, TFunc&& func) noexcept
	{
		errno = 0;
		kxf::XChar* end = nullptr;
		auto result = std::invoke(func, start, &end);

		if (end != start && errno == 0)
		{
			value = result;
			return true;
		}
		return false;
	}
}

namespace kxf
{
	const String NullString;

	std::basic_string_view<XChar> StringViewOf(const String& string) noexcept
	{
		return {string.xc_str(), string.length()};
	}

	std::basic_string_view<XChar> StringViewOf(const wxString& string) noexcept
	{
		return {string.wx_str(), string.length()};
	}

	template<class T>
	std::basic_string_view<T> StringViewOf(const wxScopedCharTypeBuffer<T>& buffer) noexcept
	{
		return {buffer.data(), buffer.length()};
	}

	template<class T>
	const auto ScopedCharBufferOf(T&& value) noexcept
	{
		auto view = StringViewOf(std::forward<T>(value));
		using CharType = typename decltype(view)::value_type;

		return wxScopedCharTypeBuffer<CharType>::CreateNonOwned(view.data(), view.length());
	}
}

namespace kxf
{
	// Comparison
	std::strong_ordering String::DoCompare(std::string_view left, std::string_view right, FlagSet<StringActionFlag> flags) noexcept
	{
		return DoCompareStrings(left, right, flags & StringActionFlag::IgnoreCase);
	}
	std::strong_ordering String::DoCompare(std::wstring_view left, std::wstring_view right, FlagSet<StringActionFlag> flags) noexcept
	{
		return DoCompareStrings(left, right, flags & StringActionFlag::IgnoreCase);
	}
	std::strong_ordering String::DoCompare(UniChar left, UniChar right, FlagSet<StringActionFlag> flags) noexcept
	{
		if (flags & StringActionFlag::IgnoreCase)
		{
			return ToLower(left) <=> ToLower(right);
		}
		else
		{
			return left <=> right;
		}
	}

	bool String::DoMatchesWildcards(std::string_view name, std::string_view expression, FlagSet<StringActionFlag> flags) noexcept
	{
		return IsNameInExpression(name, expression, flags & StringActionFlag::IgnoreCase);
	}
	bool String::DoMatchesWildcards(std::wstring_view name, std::wstring_view expression, FlagSet<StringActionFlag> flags) noexcept
	{
		return IsNameInExpression(name, expression, flags & StringActionFlag::IgnoreCase);
	}

	// Conversions
	UniChar String::FromUTF8(char c)
	{
		const char data[2] = {c, '\0'};
		String result = FromUTF8(data, 1);
		if (!result.IsEmpty())
		{
			return result.front();
		}
		return {};
	}
	String String::FromUTF8(const char* utf8, size_t length)
	{
		return EncodingConverter_UTF8.ToWideChar(utf8, length);
	}
	std::string String::ToUTF8(std::wstring_view utf16)
	{
		return EncodingConverter_UTF8.ToMultiByte(utf16);
	}

	String String::FromASCII(const char* ascii, size_t length)
	{
		return EncodingConverter_ASCII.ToWideChar(ascii, length);
	}
	String String::FromASCII(std::string_view ascii)
	{
		return EncodingConverter_ASCII.ToWideChar(ascii);
	}

	String String::FromFloatingPoint(double value, int precision)
	{
		return wxString::FromCDouble(value, precision);
	}

	// Case conversion
	UniChar String::ToLower(UniChar c) noexcept
	{
		return DoCharToUpper(static_cast<XChar>(*c));
	}
	UniChar String::ToUpper(UniChar c) noexcept
	{
		return DoCharToLower(static_cast<XChar>(*c));
	}

	bool String::IsEmptyOrWhitespace() const noexcept
	{
		if (m_String.empty())
		{
			return true;
		}
		else
		{
			for (const auto& c: m_String)
			{
				if (!std::isspace(c))
				{
					return false;
				}
			}
			return true;
		}
	}

	// Comparison
	bool String::DoStartsWith(std::string_view pattern, String* rest, FlagSet<StringActionFlag> flags) const
	{
		String patternCopy = FromUTF8(pattern);
		return StartsWith(patternCopy, rest, flags);
	}
	bool String::DoStartsWith(std::wstring_view pattern, String* rest, FlagSet<StringActionFlag> flags) const
	{
		if (pattern.empty())
		{
			return false;
		}

		flags.Remove(StringActionFlag::FromEnd);
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
	
	bool String::DoEndsWith(std::string_view pattern, String* rest, FlagSet<StringActionFlag> flags) const
	{
		String patternCopy = FromUTF8(pattern);
		return EndsWith(patternCopy, rest, flags);
	}
	bool String::DoEndsWith(std::wstring_view pattern, String* rest, FlagSet<StringActionFlag> flags) const
	{
		if (pattern.empty())
		{
			return false;
		}

		const size_t pos = Find(pattern, 0, flags|StringActionFlag::FromEnd);
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

	// Construction
	String::String(const wxString& other) noexcept
		:m_String(StringViewOf(other))
	{
	}
	String::String(wxString&& other) noexcept
	{
		Private::MoveWxString(m_String, std::move(other));
	}

	// Conversions
	std::string String::ToASCII(char replaceWith) const
	{
		std::string ascii;
		ascii.reserve(m_String.length());

		for (UniChar c: m_String)
		{
			ascii += c.ToASCII().value_or(replaceWith);
		}
		return ascii;
	}

	// Substring extraction
	String String::AfterFirst(UniChar c, String* rest, FlagSet<StringActionFlag> flags) const
	{
		const size_t pos = Find(c, 0, flags);
		if (pos != npos)
		{
			if (rest)
			{
				if (pos != 0)
				{
					*rest = SubRange(0, pos - 1);
				}
				else
				{
					rest->clear();
				}
			}
			return SubMid(pos + 1);
		}
		return {};
	}
	String String::AfterLast(UniChar c, String* rest, FlagSet<StringActionFlag> flags) const
	{
		const size_t pos = Find(c, 0, flags|StringActionFlag::FromEnd);
		if (pos != npos)
		{
			if (rest)
			{
				if (pos != 0)
				{
					*rest = SubRange(0, pos - 1);
				}
				else
				{
					rest->clear();
				}
			}
			return SubMid(pos + 1);
		}
		return {};
	}

	String String::BeforeFirst(UniChar c, String* rest, FlagSet<StringActionFlag> flags) const
	{
		const size_t pos = Find(c, 0, flags);
		if (pos != npos)
		{
			if (rest)
			{
				*rest = SubMid(pos + 1);
			}
			if (pos != 0)
			{
				return SubRange(0, pos - 1);
			}
		}
		return {};
	}
	String String::BeforeLast(UniChar c, String* rest, FlagSet<StringActionFlag> flags) const
	{
		const size_t pos = Find(c, 0, flags|StringActionFlag::FromEnd);
		if (pos != npos)
		{
			if (rest)
			{
				*rest = SubMid(pos + 1);
			}
			if (pos != 0)
			{
				return SubRange(0, pos - 1);
			}
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
	size_t String::DoFind(std::string_view pattern, size_t offset, FlagSet<StringActionFlag> flags) const
	{
		String patternCopy = FromUTF8(pattern);
		return DoFind(StringViewOf(patternCopy), offset, flags);
	}
	size_t String::DoFind(std::wstring_view pattern, size_t offset, FlagSet<StringActionFlag> flags) const
	{
		if (!m_String.empty() && offset < m_String.length())
		{
			if (flags & StringActionFlag::IgnoreCase)
			{
				wxString sourceL = StringToLower(ScopedCharBufferOf(m_String));
				wxString patternL = StringToLower(ScopedCharBufferOf(pattern));

				if (flags & StringActionFlag::FromEnd)
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
				if (flags & StringActionFlag::FromEnd)
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
	size_t String::DoFind(UniChar pattern, size_t offset, FlagSet<StringActionFlag> flags) const noexcept
	{
		if (m_String.empty() || offset >= m_String.length())
		{
			return npos;
		}

		if (flags & StringActionFlag::FromEnd)
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

	size_t String::DoReplace(std::string_view pattern, std::string_view replacement, size_t offset, FlagSet<StringActionFlag> flags)
	{
		String patternCopy = FromUTF8(pattern);
		String replacementCopy = FromUTF8(replacement);
		return Replace(StringViewOf(patternCopy), StringViewOf(replacementCopy), offset, flags);
	}
	size_t String::DoReplace(std::wstring_view pattern, std::wstring_view replacement, size_t offset, FlagSet<StringActionFlag> flags)
	{
		const size_t replacementLength = replacement.length();
		const size_t patternLength = pattern.length();

		if (m_String.empty() || patternLength == 0 || offset >= m_String.length())
		{
			return 0;
		}

		size_t replacementCount = 0;
		size_t pos = wxString::npos;

		wxString sourceL;
		wxString patternL;
		if (flags & StringActionFlag::IgnoreCase)
		{
			Private::MoveWxString(patternL, StringToLower(ScopedCharBufferOf(m_String)));
			Private::MoveWxString(patternL, StringToLower(ScopedCharBufferOf(pattern)));

			if (flags & StringActionFlag::FromEnd)
			{
				pos = sourceL.rfind(patternL, offset);
			}
			else
			{
				pos = sourceL.find(patternL, offset);
			}
		}
		else
		{
			if (flags & StringActionFlag::FromEnd)
			{
				pos = m_String.rfind(ScopedCharBufferOf(pattern), offset);
			}
			else
			{
				pos = m_String.find(ScopedCharBufferOf(pattern), offset);
			}
		}

		while (pos != wxString::npos)
		{
			m_String.replace(pos, patternLength, replacement.data(), replacement.length());
			replacementCount++;

			if (flags & StringActionFlag::FirstMatchOnly)
			{
				return replacementCount;
			}

			if (flags & StringActionFlag::IgnoreCase)
			{
				if (flags & StringActionFlag::FromEnd)
				{
					pos = sourceL.rfind(patternL, pos + replacementLength);
				}
				else
				{
					pos = sourceL.find(patternL, pos + replacementLength);
				}
			}
			else
			{
				if (flags & StringActionFlag::FromEnd)
				{
					pos = m_String.rfind(ScopedCharBufferOf(pattern), pos + replacementLength);
				}
				else
				{
					pos = m_String.find(ScopedCharBufferOf(pattern), pos + replacementLength);
				}
			}
		}
		return replacementCount;
	}
	size_t String::DoReplace(UniChar pattern, UniChar replacement, size_t offset, FlagSet<StringActionFlag> flags) noexcept
	{
		if (m_String.empty() || offset >= m_String.length())
		{
			return 0;
		}

		size_t replacementCount = 0;
		auto TestAndReplace = [&](XChar& c)
		{
			if (Compare(c, pattern, flags) == 0)
			{
				c = *replacement;
				replacementCount++;

				return !flags.Contains(StringActionFlag::FirstMatchOnly);
			}
			return true;
		};

		if (flags & StringActionFlag::FromEnd)
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

	bool String::DoContainsAnyOfCharacters(std::string_view pattern, FlagSet<StringActionFlag> flags) const noexcept
	{
		for (auto c: pattern)
		{
			if (DoFind(c, 0, flags) != npos)
			{
				return true;
			}
		}
		return false;
	}
	bool String::DoContainsAnyOfCharacters(std::wstring_view pattern, FlagSet<StringActionFlag> flags) const noexcept
	{
		for (auto c: pattern)
		{
			if (DoFind(c, 0, flags) != npos)
			{
				return true;
			}
		}
		return false;
	}

	// Conversion to numbers
	bool String::DoToFloatingPoint(float& value) const noexcept
	{
		return ConvertToFloat(value, wc_str(), std::wcstof);
	}
	bool String::DoToFloatingPoint(double& value) const noexcept
	{
		return ConvertToFloat(value, wc_str(), std::wcstod);
	}
	bool String::DoToSignedInteger(int64_t& value, int base) const noexcept
	{
		return ConvertToInteger(value, base, wc_str(), std::wcstoll);
	}
	bool String::DoToUnsignedInteger(uint64_t& value, int base) const noexcept
	{
		return ConvertToInteger(value, base, wc_str(), std::wcstoull);
	}

	// Miscellaneous
	String& String::Trim(FlagSet<StringActionFlag> flags)
	{
		wxString temp;
		Private::MoveWxString(temp, std::move(m_String));

		temp.Trim(flags & StringActionFlag::FromEnd);
		Private::MoveWxString(m_String, std::move(temp));

		return *this;
	}

	// Conversion
	String::operator wxString() const
	{
		return wxString(xc_str(), length());
	}

	// Comparison
	std::strong_ordering String::operator<=>(const wxString& other) const noexcept
	{
		return xc_view() <=> StringViewOf(other);
	}
}

namespace kxf::Private
{
	const String::string_type& GetWxStringImpl(const wxString& string) noexcept
	{
		return string.ToStdWstring();
	}
	String::string_type& GetWxStringImpl(wxString& string) noexcept
	{
		return const_cast<wxStringImpl&>(string.ToStdWstring());
	}

	void MoveWxString(wxString& destination, wxString&& source) noexcept
	{
		if (&source != &destination)
		{
			// Also see a comment in the next overload
			GetWxStringImpl(destination) = std::move(GetWxStringImpl(source));
		}
	}
	void MoveWxString(wxString& destination, String::string_type&& source) noexcept
	{
		// wxString contains an extra buffer (m_convertedTo[W]Char) to hold converted string
		// returned by 'wxString::AsCharBuf' but it seems it can be left untouched since wxString
		// always rewrites its content when requested to make conversion and only changes its size
		// when needed.

		if (&source != &GetWxStringImpl(destination))
		{
			GetWxStringImpl(destination) = std::move(source);
		}
	}
	void MoveWxString(String::string_type& destination, wxString&& source) noexcept
	{
		if (&GetWxStringImpl(source) != &destination)
		{
			// Also see a comment in the next overload
			destination = std::move(GetWxStringImpl(source));
		}
	}
}

namespace kxf
{
	uint64_t BinarySerializer<String>::Serialize(IOutputStream& stream, const String& value) const
	{
		return Serialization::WriteObject(stream, value.ToUTF8());
	}
	uint64_t BinarySerializer<String>::Deserialize(IInputStream& stream, String& value) const
	{
		std::string buffer;
		auto read = Serialization::ReadObject(stream, buffer);
		value = String::FromUTF8(buffer);

		return read;
	}
}
