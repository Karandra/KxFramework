#pragma once
#include "Common.h"
#include "Private/String.h"
#include <string>
#include <string_view>
#include "kxf/System/UndefWindows.h"

namespace kxf
{
	using XChar = wxChar;
	using StringView = std::basic_string_view<XChar>;
	extern const String NullString;

	enum class StringOpFlag: uint32_t
	{
		None = 0,
		IgnoreCase = 1 << 0,
		FromEnd = 1 << 1,
		FirstMatchOnly = 1 << 2,
	};
	KxFlagSet_Declare(StringOpFlag);

	namespace StringFormatter
	{
		template<class T>
		class Formatter;
	}
}

namespace kxf
{
	template<class T>
	std::basic_string_view<T> StringViewOf(const std::basic_string<T>& string) noexcept
	{
		return {string.data(), string.length()};
	}

	template<class T>
	std::basic_string_view<T> StringViewOf(std::basic_string_view<T> view) noexcept
	{
		return view;
	}

	template<class T>
	std::basic_string_view<T> StringViewOf(const wxScopedCharTypeBuffer<T>& buffer) noexcept
	{
		return {buffer.data(), buffer.length()};
	}

	template<class T>
	std::enable_if_t<std::is_same_v<T, wxString> || std::is_same_v<T, String>, std::basic_string_view<XChar>> StringViewOf(const T& string) noexcept
	{
		return {string.wx_str(), string.length()};
	}

	template<class T>
	std::enable_if_t<std::is_same_v<T, char> || std::is_same_v<T, wchar_t>, std::basic_string_view<T>> StringViewOf(const T* ptr) noexcept
	{
		if (ptr)
		{
			return ptr;
		}
		return {};
	}

	template<class T, size_t size>
	std::enable_if_t<std::is_array_v<T>, std::basic_string_view<T>> StringViewOf(const T(&buffer)[size]) noexcept
	{
		return buffer;
	}

	template<class T>
	const auto ScopedCharBufferOf(T&& value) noexcept
	{
		auto view = StringViewOf(std::forward<T>(value));
		using CharType = typename decltype(view)::value_type;

		return wxScopedCharTypeBuffer<CharType>::CreateNonOwned(view.data(), view.length());
	}

	inline wxUniChar UniCharOf(wxUniChar c) noexcept
	{
		return c;
	}
	inline wxUniChar UniCharOf(wxUniCharRef c) noexcept
	{
		return c;
	}
	inline wxUniChar UniCharOf(char c) noexcept
	{
		return c;
	}
	inline wxUniChar UniCharOf(wchar_t c) noexcept
	{
		return c;
	}
}

namespace kxf
{
	class KX_API String final
	{
		public:
			using traits_type = std::char_traits<XChar>;
			using allocator_type = std::allocator<XChar>;
			
			using iterator = wxString::iterator;
			using const_iterator = wxString::const_iterator;
			using reverse_iterator = wxString::reverse_iterator;
			using const_reverse_iterator = wxString::const_reverse_iterator;

		public:
			static constexpr size_t npos = StringView::npos;

			// Comparison
		private:
			static int DoCompare(std::string_view left, std::string_view right, FlagSet<StringOpFlag> flags = {}) noexcept;
			static int DoCompare(std::wstring_view left, std::wstring_view right, FlagSet<StringOpFlag> flags = {}) noexcept;
			static int DoCompare(wxUniChar left, wxUniChar right, FlagSet<StringOpFlag> flags = {}) noexcept;
			
		public:
			template<class T1, class T2>
			static int Compare(T1&& left, T2&& right, FlagSet<StringOpFlag> flags = {}) noexcept
			{
				if constexpr(Private::IsAnyCharType<T1>() && Private::IsAnyCharType<T2>())
				{
					return DoCompare(UniCharOf(std::forward<T1>(left)), UniCharOf(std::forward<T2>(right)), flags);
				}
				else
				{
					return DoCompare(StringViewOf(std::forward<T1>(left)), StringViewOf(std::forward<T2>(right)), flags);
				}
			}

		private:
			static bool DoMatchesWildcards(std::string_view name, std::string_view expression, FlagSet<StringOpFlag> flags = {}) noexcept;
			static bool DoMatchesWildcards(std::wstring_view name, std::wstring_view expression, FlagSet<StringOpFlag> flags = {}) noexcept;

		public:
			template<class T1, class T2>
			static int MatchesWildcards(T1&& name, T2&& expression, FlagSet<StringOpFlag> flags = {}) noexcept
			{
				return DoMatchesWildcards(StringViewOf(std::forward<T1>(name)), StringViewOf(std::forward<T2>(expression)), flags);
			}

			// Conversions
			static String FromView(const std::string_view view)
			{
				return String(view);
			}
			static String FromView(const std::wstring_view view)
			{
				return String(view);
			}

			static wxUniChar FromUTF8(char c)
			{
				const char data[2] = {c, '\0'};
				wxString result = wxString::FromUTF8(data, 1);
				if (!result.IsEmpty())
				{
					return result[0];
				}
				return {};
			}
			static String FromUTF8(const char* utf8, size_t length = npos)
			{
				return wxString::FromUTF8(utf8, length);
			}
			static String FromUTF8(const std::string& utf8)
			{
				return wxString::FromUTF8(utf8.data(), utf8.length());
			}
			static String FromUTF8(std::string_view utf8)
			{
				return wxString::FromUTF8(utf8.data(), utf8.length());
			}
			static String FromUTF8(const wxScopedCharBuffer& utf8)
			{
				return wxString::FromUTF8(utf8.data(), utf8.length());
			}

			static wxUniChar FromASCII(char c)
			{
				wxString result = wxString::FromAscii(c);
				if (!result.IsEmpty())
				{
					return result[0];
				}
				return {};
			}
			static String FromASCII(const char* ascii, size_t length = npos)
			{
				return wxString::FromAscii(ascii, length);
			}
			static String FromASCII(std::string_view ascii)
			{
				return wxString::FromAscii(ascii.data(), ascii.length());
			}
			static String FromASCII(const wxScopedCharBuffer& ascii)
			{
				return wxString::FromAscii(ascii.data(), ascii.length());
			}

			static String From8BitData(const char* binaryData, size_t length = npos)
			{
				if (length != npos)
				{
					return wxString::From8BitData(binaryData, length);
				}
				return wxString::From8BitData(binaryData);
			}
			static String From8BitData(const wxScopedCharBuffer& binaryData)
			{
				return wxString::From8BitData(binaryData.data(), binaryData.length());
			}

			static String FromDouble(double value, int precision = -1)
			{
				return wxString::FromDouble(value, precision);
			}
			static String FromCDouble(double value, int precision = -1)
			{
				return wxString::FromCDouble(value, precision);
			}

			// Case conversion
			static wxUniChar ToLower(wxUniChar c) noexcept;
			static wxUniChar ToUpper(wxUniChar c) noexcept;

			// Concatenation
			template<class... Args>
			static String Concat(Args&&... arg)
			{
				return (String(std::forward<Args>(arg)) + ...);
			}

			template<class... Args>
			static String ConcatWithSeparator(const String& sep, Args&&... arg)
			{
				String value = ((String(std::forward<Args>(arg)) + sep) + ...);
				value.RemoveFromEnd(sep.length());
				return value;
			}

			// Substring extraction
			template<class TFunc>
			static size_t SplitBySeparator(const String& string, const String& sep, TFunc&& func, FlagSet<StringOpFlag> flags = {})
			{
				const StringView view = string.GetView();

				if (sep.empty() && !string.empty())
				{
					std::invoke(func, view);
					return 1;
				}

				size_t separatorPos = string.Find(sep, 0, flags);
				if (separatorPos == String::npos)
				{
					std::invoke(func, view);
					return 1;
				}

				size_t pos = 0;
				size_t count = 0;
				while (pos < string.length() && separatorPos <= string.length())
				{
					StringView stringPiece = view.substr(pos, separatorPos - pos);
					const size_t stringPieceLength = stringPiece.length();

					if (!stringPiece.empty())
					{
						count++;
						if (!std::invoke(func, std::move(stringPiece)))
						{
							return count;
						}
					}

					pos += stringPieceLength + sep.length();
					separatorPos = string.Find(sep, pos, flags);

					// No separator found, but this is not the last element
					if (separatorPos == String::npos && pos < string.length())
					{
						separatorPos = string.length();
					}
				}
				return count;
			}

			template<class TFunc>
			static size_t SplitByLength(const String& string, size_t length, TFunc&& func)
			{
				if (length != 0)
				{
					const StringView view = string.GetView();

					size_t count = 0;
					for (size_t i = 0; i < view.length(); i += length)
					{
						StringView stringPiece = view.substr(i, length);
						if (!stringPiece.empty())
						{
							count++;
							if (!std::invoke(func, std::move(stringPiece)))
							{
								return count;
							}
						}
					}
					return count;
				}
				else
				{
					std::invoke(func, string.GetView());
					return 1;
				}
				return 0;
			}

			// Formatting
			template<class TString, class... Args>
			static String Format(TString&& format, Args&&... arg)
			{
				if constexpr((sizeof...(Args)) != 0)
				{
					StringFormatter::Formatter formatter(std::forward<TString>(format));
					std::initializer_list<int>{((void)formatter(arg), 0) ...};
					return formatter;
				}
				return format;
			}

			template<class Traits, class TString, class... Args>
			static String Format(TString&& format, Args&&... arg)
			{
				if constexpr ((sizeof...(Args)) != 0)
				{
					StringFormatter::Formatter<Traits> formatter(std::forward<TString>(format));
					std::initializer_list<int>{((void)formatter(arg), 0) ...};
					return formatter;
				}
				return format;
			}

		private:
			wxString m_String;

		public:
			String() = default;

			// Copy
			String(const String&) = default;
			String(const wxString& other)
				:m_String(other)
			{
			}
			
			// Move
			String(String&& other) noexcept(Private::IsWxStringMoveable())
			{
				*this = std::move(other);
			}
			String(wxString&& other) noexcept(Private::IsWxStringMoveable())
			{
				*this = std::move(other);
			}

			#if Kx_WxStringConvertibleToStd
			String(wxStringImpl&& other) noexcept(Private::IsWxStringMoveable())
			{
				*this = std::move(other);
			}
			#endif

			// Char/wchar_t pointers
			String(const char* data, size_t length = npos)
				:m_String(data, Private::CheckStringLength(data, length))
			{
			}
			String(const char* data, const wxMBConv& conv, size_t length = npos)
				:m_String(data, conv, Private::CheckStringLength(data, length))
			{
			}
			String(const wchar_t* data, size_t length = npos)
				:m_String(data, Private::CheckStringLength(data, length))
			{
			}
			
			// std::[w]string_view
			explicit String(std::string_view other)
				:m_String(other.data(), other.length())
			{
			}
			explicit String(std::string_view other, const wxMBConv& conv)
				:m_String(other.data(), conv, other.length())
			{
			}
			explicit String(std::wstring_view other)
				:m_String(other.data(), other.length())
			{
			}

			// Single char
			String(char c, size_t count = 1)
				:m_String(c, count)
			{
			}
			String(wchar_t c, size_t count = 1)
				:m_String(c, count)
			{
			}
			String(wxUniChar c, size_t count = 1)
				:m_String(c, count)
			{
			}
			String(wxUniCharRef c, size_t count = 1)
				:m_String(c, count)
			{
			}
			
			~String() noexcept = default;

		public:
			// String length
			bool IsEmpty() const noexcept
			{
				return m_String.IsEmpty();
			}
			bool IsEmptyOrWhitespace() const noexcept;
			size_t GetLength() const noexcept
			{
				return m_String.length();
			}
			size_t GetCapacity() const noexcept
			{
				return m_String.capacity();
			}

			// Character access
			StringView GetView() const noexcept
			{
				return StringViewOf(m_String);
			}
			auto c_str() const noexcept(std::is_same_v<XChar, char>)
			{
				return m_String.c_str();
			}
			auto wc_str() const noexcept(std::is_same_v<XChar, wchar_t>)
			{
				return m_String.wc_str();
			}
			const XChar* wx_str() const noexcept
			{
				return m_String.wx_str();
			}

			wxUniChar operator[](size_t i) const
			{
				return m_String[i];
			}
			wxUniCharRef operator[](size_t i)
			{
				return m_String[i];
			}

			const wxString& GetWxString() const noexcept
			{
				return m_String;
			}
			wxString& GetWxString() noexcept
			{
				return m_String;
			}
			
			#if Kx_WxStringConvertibleToStd
			const wxStringImpl& GetStdImpl() const noexcept
			{
				return Private::GetWxStringImpl(m_String);
			}
			wxStringImpl& GetStdImpl() noexcept
			{
				return Private::GetWxStringImpl(m_String);
			}
			#endif

			// Conversions
			wxString TakeWxString() noexcept
			{
				wxString result;
				Private::MoveWxString(result, std::move(m_String));
				return result;
			}
			std::string ToStdString(const wxMBConv& conv = wxConvLibc) const
			{
				return m_String.ToStdString(conv);
			}
			std::wstring ToStdWString() const
			{
				return m_String.ToStdWstring();
			}
			std::string ToStdStringUTF8() const
			{
				auto utf8 = m_String.ToUTF8();
				return std::string(utf8.data(), utf8.length());
			}
			const wxScopedCharBuffer ToUTF8() const
			{
				return m_String.ToUTF8();
			}
			const wxScopedCharBuffer ToASCII(char replaceWith = '_') const
			{
				return m_String.ToAscii();
			}
			const wxScopedCharBuffer To8BitData() const
			{
				return m_String.To8BitData();
			}

			// Concatenation
		private:
			String& DoAppend(std::string_view other)
			{
				m_String.Append(other.data(), other.length());
				return *this;
			}
			String& DoAppend(std::wstring_view other)
			{
				m_String.Append(other.data(), other.length());
				return *this;
			}
			String& DoAppend(wxUniChar c, size_t count = 1)
			{
				m_String.Append(c, count);
				return *this;
			}

		public:
			template<class T>
			std::enable_if_t<Private::IsAnyStringType<T>(), String&> Append(T&& other)
			{
				return DoAppend(StringViewOf(std::forward<T>(other)));
			}

			template<class T>
			std::enable_if_t<Private::IsAnyCharType<T>(), String&> Append(T&& other, size_t count = 1)
			{
				return DoAppend(UniCharOf(std::forward<T>(other)), count);
			}

			template<class T>
			String& operator+=(T&& other)
			{
				return Append(std::forward<T>(other));
			}

			template<class T>
			String& operator<<(T&& other)
			{
				return Append(std::forward<T>(other));
			}

		private:
			String& DoPrepend(std::string_view other)
			{
				m_String.insert(0, other.data(), other.length());
				return *this;
			}
			String& DoPrepend(std::wstring_view other)
			{
				m_String.insert(0, other.data(), other.length());
				return *this;
			}
			String& DoPrepend(wxUniChar c, size_t count = 1)
			{
				m_String.insert(0, count, c);
				return *this;
			}
			
		public:
			template<class T>
			std::enable_if_t<Private::IsAnyStringType<T>(), String&> Prepend(T&& other)
			{
				return DoPrepend(StringViewOf(std::forward<T>(other)));
			}

			template<class T>
			std::enable_if_t<Private::IsAnyCharType<T>(), String&> Prepend(T&& other, size_t count = 1)
			{
				return DoPrepend(UniCharOf(std::forward<T>(other)), count);
			}

		private:
			String& DoInsert(size_t pos, std::string_view other)
			{
				m_String.insert(pos, other.data(), other.length());
				return *this;
			}
			String& DoInsert(size_t pos, std::wstring_view other)
			{
				m_String.insert(pos, other.data(), other.length());
				return *this;
			}
			String& DoInsert(size_t pos, wxUniChar c, size_t count = 1)
			{
				m_String.insert(pos, count, c);
				return *this;
			}
			
		public:
			template<class T>
			std::enable_if_t<Private::IsAnyStringType<T>(), String&> Insert(size_t pos, T&& other)
			{
				return DoInsert(pos, StringViewOf(std::forward<T>(other)));
			}

			template<class T>
			std::enable_if_t<Private::IsAnyCharType<T>(), String&> Insert(size_t pos, T&& other, size_t count = 1)
			{
				return DoInsert(pos, UniCharOf(std::forward<T>(other)), count);
			}

			// Comparison
		private:
			int DoCompareTo(std::string_view other, FlagSet<StringOpFlag> flags = {}) const noexcept(std::is_same_v<XChar, char>)
			{
				#if wxUSE_UNICODE_WCHAR
				return Compare(*this, String(other), flags);
				#else
				return Compare(GetView(), other, flags);
				#endif
			}
			int DoCompareTo(std::wstring_view other, FlagSet<StringOpFlag> flags = {}) const noexcept(std::is_same_v<XChar, wchar_t>)
			{
				#if wxUSE_UNICODE_WCHAR
				return Compare(GetView(), other, flags);
				#else
				return Compare(*this, String(other), flags);
				#endif
			}
			int DoCompareTo(wxUniChar other, FlagSet<StringOpFlag> flags = {}) const noexcept
			{
				const XChar c[2] = {other, 0};
				return Compare(GetView(), StringViewOf(c), flags);
			}

		public:
			template<class T>
			int CompareTo(T&& other, FlagSet<StringOpFlag> flags = {}) const
			{
				if constexpr(Private::IsAnyCharType<T>())
				{
					return DoCompareTo(UniCharOf(std::forward<T>(other)), flags);
				}
				else
				{
					return DoCompareTo(StringViewOf(std::forward<T>(other)), flags);
				}
			}

			template<class T>
			bool IsSameAs(T&& other, FlagSet<StringOpFlag> flags = {}) const
			{
				return CompareTo(std::forward<T>(other), flags) == 0;
			}

		private:
			bool DoStartsWith(std::string_view pattern, String* rest = nullptr, FlagSet<StringOpFlag> flags = {}) const;
			bool DoStartsWith(std::wstring_view pattern, String* rest = nullptr, FlagSet<StringOpFlag> flags = {}) const;
			bool DoStartsWith(wxUniChar c, String* rest = nullptr, FlagSet<StringOpFlag> flags = {}) const noexcept
			{
				const XChar pattern[2] = {c, 0};
				return StartsWith(StringViewOf(pattern), rest, flags);
			}

		public:
			template<class T>
			bool StartsWith(T&& pattern, String* rest = nullptr, FlagSet<StringOpFlag> flags = {}) const
			{
				if constexpr(Private::IsAnyCharType<T>())
				{
					return DoStartsWith(UniCharOf(std::forward<T>(pattern)), rest, flags);
				}
				else
				{
					return DoStartsWith(StringViewOf(std::forward<T>(pattern)), rest, flags);
				}
			}

		private:
			bool DoEndsWith(std::string_view pattern, String* rest = nullptr, FlagSet<StringOpFlag> flags = {}) const;
			bool DoEndsWith(std::wstring_view pattern, String* rest = nullptr, FlagSet<StringOpFlag> flags = {}) const;
			bool DoEndsWith(wxUniChar c, String* rest = nullptr, FlagSet<StringOpFlag> flags = {}) const noexcept
			{
				const XChar pattern[2] = {c, 0};
				return EndsWith(StringViewOf(pattern), rest, flags);
			}

		public:
			template<class T>
			bool EndsWith(T&& pattern, String* rest = nullptr, FlagSet<StringOpFlag> flags = {}) const
			{
				if constexpr(Private::IsAnyCharType<T>())
				{
					return DoEndsWith(UniCharOf(std::forward<T>(pattern)), rest, flags);
				}
				else
				{
					return DoEndsWith(StringViewOf(std::forward<T>(pattern)), rest, flags);
				}
			}

		private:
			bool DoMatchesWildcards(std::string_view expression, FlagSet<StringOpFlag> flags = {}) const noexcept
			{
				#if wxUSE_UNICODE_WCHAR
				String temp = FromView(expression);
				return DoMatchesWildcards(GetView(), StringViewOf(temp), flags);
				#else
				return DoMatchesWildcards(GetView(), expression, flags);
				#endif
			}
			bool DoMatchesWildcards(std::wstring_view expression, FlagSet<StringOpFlag> flags = {}) const noexcept
			{
				#if wxUSE_UNICODE_WCHAR
				return DoMatchesWildcards(GetView(), expression, flags);
				#else
				String temp = FromView(expression);
				return DoMatchesWildcards(GetView(), StringViewOf(temp), flags);
				#endif
			}
			bool DoMatchesWildcards(wxUniChar c, FlagSet<StringOpFlag> flags = {}) const noexcept
			{
				const XChar expression[2] = {c, 0};
				return DoMatchesWildcards(GetView(), StringViewOf(expression), flags);
			}

		public:
			template<class T>
			bool MatchesWildcards(T&& expression, FlagSet<StringOpFlag> flags = {}) const
			{
				if constexpr(Private::IsAnyCharType<T>())
				{
					return DoMatchesWildcards(UniCharOf(std::forward<T>(expression)), flags);
				}
				else
				{
					return DoMatchesWildcards(StringViewOf(std::forward<T>(expression)), flags);
				}
			}

			// Substring extraction
			String Mid(size_t offset, size_t count = String::npos) const
			{
				return m_String.Mid(offset, count);
			}
			String Left(size_t count) const
			{
				return m_String.Left(count);
			}
			String Right(size_t count) const
			{
				return m_String.Right(count);
			}
			String SubString(size_t from, size_t to) const
			{
				return m_String.SubString(from, to);
			}

			String AfterFirst(wxUniChar c, String* rest = nullptr, FlagSet<StringOpFlag> flags = {}) const;
			String AfterLast(wxUniChar c, String* rest = nullptr, FlagSet<StringOpFlag> flags = {}) const;

			String BeforeFirst(wxUniChar c, String* rest = nullptr, FlagSet<StringOpFlag> flags = {}) const;
			String BeforeLast(wxUniChar c, String* rest = nullptr, FlagSet<StringOpFlag> flags = {}) const;

			template<class TFunc>
			size_t SplitBySeparator(const String& sep, TFunc&& func, FlagSet<StringOpFlag> flags = {}) const
			{
				return SplitBySeparator(*this, sep, std::forward<TFunc>(func), flags);
			}

			template<class TFunc>
			size_t SplitByLength(size_t length, TFunc&& func) const
			{
				return SplitByLength(StringViewOf(m_String), length, std::forward<TFunc>(func));
			}

			// Case conversion
			String& MakeLower() noexcept;
			String& MakeUpper() noexcept;
			String ToLower() const
			{
				return Clone().MakeLower();
			}
			String ToUpper() const
			{
				return Clone().MakeUpper();
			}

			String& MakeCapitalized() noexcept
			{
				if (!m_String.IsEmpty())
				{
					m_String[0] = ToUpper(m_String[0]);
				}
				return *this;
			}
			String Capitalize() const
			{
				return Clone().MakeCapitalized();
			}

			// Searching and replacing
		private:
			size_t DoFind(std::string_view pattern, size_t offset = 0, FlagSet<StringOpFlag> flags = {}) const;
			size_t DoFind(std::wstring_view pattern, size_t offset = 0, FlagSet<StringOpFlag> flags = {}) const;
			size_t DoFind(wxUniChar pattern, size_t offset = 0, FlagSet<StringOpFlag> flags = {}) const noexcept;
			
		public:
			template<class T>
			size_t Find(T&& pattern, size_t offset = 0, FlagSet<StringOpFlag> flags = {}) const
			{
				if constexpr(Private::IsAnyCharType<T>())
				{
					return DoFind(UniCharOf(std::forward<T>(pattern)), offset, flags);
				}
				else
				{
					return DoFind(StringViewOf(std::forward<T>(pattern)), offset, flags);
				}
			}

		private:
			size_t DoReplace(std::string_view pattern, std::string_view replacement, size_t offset = 0, FlagSet<StringOpFlag> flags = {});
			size_t DoReplace(std::wstring_view pattern, std::wstring_view replacement, size_t offset = 0, FlagSet<StringOpFlag> flags = {});
			size_t DoReplace(wxUniChar c, wxUniChar replacement, size_t offset = 0, FlagSet<StringOpFlag> flags = {}) noexcept;
			size_t DoReplace(wxUniChar c, std::string_view replacement, size_t offset = 0, FlagSet<StringOpFlag> flags = {}) noexcept
			{
				const char pattern[2] = {c, 0};
				return Replace(StringViewOf(pattern), replacement, offset, flags);
			}
			size_t DoReplace(wxUniChar c, std::wstring_view replacement, size_t offset = 0, FlagSet<StringOpFlag> flags = {}) noexcept
			{
				const wchar_t pattern[2] = {c, 0};
				return Replace(StringViewOf(pattern), replacement, offset, flags);
			}
			
		public:
			template<class T1, class T2>
			size_t Replace(T1&& pattern, T2&& replacement, size_t offset = 0, FlagSet<StringOpFlag> flags = {})
			{
				if constexpr(Private::IsAnyStringType<T1>() && Private::IsAnyStringType<T2>())
				{
					return DoReplace(StringViewOf(std::forward<T1>(pattern)), StringViewOf(std::forward<T2>(replacement)), offset, flags);
				}
				else if constexpr(Private::IsAnyCharType<T1>() && Private::IsAnyCharType<T2>())
				{
					return DoReplace(UniCharOf(std::forward<T1>(pattern)), UniCharOf(std::forward<T2>(replacement)), offset, flags);
				}
				else if constexpr(Private::IsAnyCharType<T1>() && Private::IsAnyStringType<T2>())
				{
					return DoReplace(UniCharOf(std::forward<T1>(pattern)), StringViewOf(std::forward<T2>(replacement)), offset, flags);
				}
				else
				{
					static_assert(false, "invalid argument types");
				}
			}

			String& ReplaceRange(size_t offset, size_t length, const String& replacement)
			{
				m_String.replace(offset, length, replacement);
				return *this;
			}
			String& ReplaceRange(size_t offset, size_t length, std::string_view replacement)
			{
				m_String.replace(offset, length, replacement.data(), replacement.size());
				return *this;
			}
			String& ReplaceRange(size_t offset, size_t length, std::wstring_view replacement)
			{
				m_String.replace(offset, length, replacement.data(), replacement.size());
				return *this;
			}

			String& ReplaceRange(iterator first, iterator last, const String& replacement)
			{
				m_String.replace(first, last, replacement);
				return *this;
			}
			String& ReplaceRange(iterator first, iterator last, std::string_view replacement)
			{
				m_String.replace(first, last, replacement.data(), replacement.size());
				return *this;
			}
			String& ReplaceRange(iterator first, iterator last, std::wstring_view replacement)
			{
				m_String.replace(first, last, replacement.data(), replacement.size());
				return *this;
			}

			template<class T>
			bool Contains(T&& pattern, FlagSet<StringOpFlag> flags = {}) const
			{
				return Find(std::forward<T>(pattern), 0, flags) != npos;
			}

		private:
			bool DoContainsAnyOfCharacters(std::string_view pattern, FlagSet<StringOpFlag> flags = {}) const noexcept;
			bool DoContainsAnyOfCharacters(std::wstring_view pattern, FlagSet<StringOpFlag> flags = {}) const noexcept;

		public:
			template<class T>
			bool ContainsAnyOfCharacters(T&& pattern, FlagSet<StringOpFlag> flags = {}) const noexcept
			{
				return DoContainsAnyOfCharacters(StringViewOf(std::forward<T>(pattern)), flags);
			}

			// Conversion to numbers
			template<class T = double>
			std::optional<T> ToFloatingPoint() const noexcept
			{
				static_assert(std::is_floating_point_v<T>, "floating point type is required");

				double value = 0;
				if (m_String.ToDouble(&value))
				{
					return static_cast<T>(value);
				}
				return {};
			}

			template<class T = double>
			std::optional<T> ToCFloatingPoint() const noexcept
			{
				static_assert(std::is_floating_point_v<T>, "floating point type is required");

				double value = 0;
				if (m_String.ToCDouble(&value))
				{
					return static_cast<T>(value);
				}
				return {};
			}

			template<class T>
			std::optional<T> ToInt(int base = 10) const noexcept
			{
				static_assert(std::is_integral_v<T>, "integral type is required");

				using Limits = std::numeric_limits<T>;
				using Tint = std::conditional_t<std::is_unsigned_v<T>, unsigned long long, long long>;

				Tint value = 0;
				bool isSuccess = false;
				if constexpr(std::is_unsigned_v<T>)
				{
					isSuccess = m_String.ToULongLong(&value, base);
				}
				else
				{
					isSuccess = m_String.ToLongLong(&value, base);
				}

				if (isSuccess && value == std::clamp<Tint>(value, Limits::min(), Limits::max()))
				{
					return static_cast<T>(value);
				}
				return {};
			}

			// Memory management
			String Clone() const
			{
				return m_String;
			}
			void Clear() noexcept
			{
				m_String.Clear();
			}
			void Reserve(size_t capacity)
			{
				m_String.reserve(capacity);
			}
			void Shrink()
			{
				#if Kx_WxStringConvertibleToStd
				GetStdImpl().shrink_to_fit();
				#else
				m_String.shrink_to_fit();
				#endif
			}

			// Miscellaneous
			bool IsASCII() const noexcept
			{
				for (const auto& c: m_String)
				{
					if (!c.IsAscii())
					{
						return false;
					}
				}
				return true;
			}
			String& Remove(size_t pos, size_t count)
			{
				m_String.Remove(pos, count);
				return *this;
			}
			String& RemoveFromEnd(size_t count)
			{
				m_String.RemoveLast(count);
				return *this;
			}
			String& Truncate(size_t length)
			{
				m_String.Truncate(length);
				return *this;
			}
			String& Trim(FlagSet<StringOpFlag> flags = {})
			{
				m_String.Trim(flags & StringOpFlag::FromEnd);
				return *this;
			}

			// Iterator interface
			iterator begin() noexcept
			{
				return m_String.begin();
			}
			iterator end() noexcept
			{
				return m_String.end();
			}
			const_iterator begin() const noexcept
			{
				return m_String.begin();
			}
			const_iterator end() const noexcept
			{
				return m_String.end();
			}
			const_iterator cbegin() const noexcept
			{
				return m_String.cbegin();
			}
			const_iterator cend() const noexcept
			{
				return m_String.cend();
			}
			
			reverse_iterator rbegin() noexcept
			{
				return m_String.rbegin();
			}
			reverse_iterator rend() noexcept
			{
				return m_String.rend();
			}
			const_reverse_iterator rbegin() const noexcept
			{
				return m_String.rbegin();
			}
			const_reverse_iterator rend() const noexcept
			{
				return m_String.rend();
			}
			const_reverse_iterator crend() const noexcept
			{
				return m_String.crend();
			}
			const_reverse_iterator crbegin() const noexcept
			{
				return m_String.crbegin();
			}

			// STL interface (incomplete)
			bool empty() const noexcept
			{
				return m_String.empty();
			}
			size_t size() const noexcept
			{
				return m_String.size();
			}
			size_t length() const noexcept
			{
				return m_String.length();
			}
			size_t capacity() const noexcept
			{
				return m_String.capacity();
			}
			size_t max_size() const noexcept
			{
				return m_String.max_size();
			}
			void clear() noexcept
			{
				m_String.clear();
			}
			void reserve(size_t capacity)
			{
				m_String.reserve(capacity);
			}
			void resize(size_t capacity, wxUniChar c = '\0')
			{
				m_String.resize(capacity, c);
			}
			void shrink_to_fit()
			{
				Shrink();
			}

			wxUniChar at(size_t i) const
			{
				return m_String.at(i);
			}
			wxUniCharRef at(size_t i)
			{
				return m_String.at(i);
			}

			wxUniChar front() const
			{
				return m_String[0];
			}
			wxUniCharRef front()
			{
				return m_String[0];
			}

			wxUniChar back() const
			{
				return m_String[m_String.length() - 1];
			}
			wxUniCharRef back()
			{
				return m_String[m_String.length() - 1];
			}

		public:
			// Copy assignment
			String& operator=(const String&) = default;
			String& operator=(const wxString& other)
			{
				m_String = other;
				return *this;
			}
			String& operator=(std::string_view other)
			{
				m_String = ScopedCharBufferOf(other);
				return *this;
			}
			String& operator=(std::wstring_view other)
			{
				m_String = ScopedCharBufferOf(other);
				return *this;
			}
			
			template<class T>
			String& operator=(const T& other)
			{
				m_String = other;
				return *this;
			}

			// Move assignment
			String& operator=(String&& other) noexcept(Private::IsWxStringMoveable())
			{
				Private::MoveWxString(m_String, std::move(other.m_String));
				return *this;
			}
			String& operator=(wxString&& other) noexcept(Private::IsWxStringMoveable())
			{
				Private::MoveWxString(m_String, std::move(other));
				return *this;
			}

			#if Kx_WxStringConvertibleToStd
			String& operator=(wxStringImpl&& other) noexcept(Private::IsWxStringMoveable())
			{
				Private::MoveWxString(m_String, std::move(other));
				return *this;
			}
			#endif

			// Conversions
			operator const wxString&() const
			{
				return GetWxString();
			}
			operator wxString&()
			{
				return GetWxString();
			}
	};
}

namespace kxf
{
	// Comparison with char
	inline bool operator==(const String& left, wxUniChar right)
	{
		return left.IsSameAs(right);
	}
	inline bool operator==(const String& left, wxUniCharRef right)
	{
		return left.IsSameAs(right);
	}
	inline bool operator==(const String& left, char right)
	{
		return left.IsSameAs(right);
	}
	inline bool operator==(const String& left, wchar_t right)
	{
		return left.IsSameAs(right);
	}
	inline bool operator==(wxUniChar left, const String& right)
	{
		return right.IsSameAs(left);
	}
	inline bool operator==(wxUniCharRef left, const String& right)
	{
		return right.IsSameAs(left);
	}
	inline bool operator==(char left, const String& right)
	{
		return right.IsSameAs(left);
	}
	inline bool operator==(wchar_t left, const String& right)
	{
		return right.IsSameAs(left);
	}
	
	inline bool operator!=(const String& left, wxUniChar right)
	{
		return !left.IsSameAs(right);
	}
	inline bool operator!=(const String& left, wxUniCharRef right)
	{
		return !left.IsSameAs(right);
	}
	inline bool operator!=(const String& left, char right)
	{
		return !left.IsSameAs(right);
	}
	inline bool operator!=(const String& left, wchar_t right)
	{
		return !left.IsSameAs(right);
	}
	inline bool operator!=(wxUniChar left, const String& right)
	{
		return !right.IsSameAs(left);
	}
	inline bool operator!=(wxUniCharRef left, const String& right)
	{
		return !right.IsSameAs(left);
	}
	inline bool operator!=(char left, const String& right)
	{
		return !right.IsSameAs(left);
	}
	inline bool operator!=(wchar_t left, const String& right)
	{
		return !right.IsSameAs(left);
	}

	// Use wxWidgets macro to quickly define all comparison operators at once.
	// Hopefully the C++20 three-way "spaceship" operator will simplify this.
	// For now this wall of code is required.

	// Disable macro redefinition warning
	#pragma warning(push, 0)   
	#pragma warning(disable: 4005)

	// String <=> String
	// Can't use 'wxDEFINE_ALL_COMPARISONS' for the same types because it'll cause multiple
	// definitions for some operators because types are the same on the left and on the right. 
	#define Kx_StringCmp(left, right, op)	left.GetWxString() op right.GetWxString()
	wxFOR_ALL_COMPARISONS_3(wxDEFINE_COMPARISON, const String&, const String&, Kx_StringCmp);

	// String <=> wxString
	#define Kx_StringCmp(left, right, op)	left.GetWxString() op right
	wxDEFINE_ALL_COMPARISONS(const String&, const wxString&, Kx_StringCmp);

	// String <=> const char*
	#define Kx_StringCmp(left, right, op)	left.GetWxString() op right
	wxDEFINE_ALL_COMPARISONS(const String&, const char* , Kx_StringCmp);

	// String <=> const wchar_t*
	#define Kx_StringCmp(left, right, op)	left.GetWxString() op right
	wxDEFINE_ALL_COMPARISONS(const String&, const wchar_t*, Kx_StringCmp);

	#undef Kx_StringCmp

	// Restore any disabled warnings
	#pragma warning(pop)

	// Concatenation
	template<class T>
	String operator+(const String& left, T&& right)
	{
		return left.Clone().Append(std::forward<T>(right));
	}
}

namespace std
{
	template<>
	struct hash<kxf::String> final
	{
		size_t operator()(const kxf::String& string) const noexcept
		{
			return std::hash<kxf::StringView>()(string.GetView());
		}
	};
}
