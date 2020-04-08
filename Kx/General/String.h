#pragma once
#include "Common.h"
#include "Private/String.h"
#include <string>
#include <string_view>
#include "Kx/System/UndefWindows.h"

namespace KxFramework
{
	using XChar = wxChar;
	using StringView = std::basic_string_view<XChar>;

	enum class StringOpFlag
	{
		None = 0,
		IgnoreCase = 1 << 0,
		FromEnd = 1 << 1,
		FirstMatchOnly = 1 << 2,
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowEverything(StringOpFlag);
	}
}

namespace KxFramework
{
	template<class T>
	std::basic_string_view<T> StringViewOf(const std::basic_string<T>& string) noexcept
	{
		return {string.data(), string.length()};
	}

	template<class T>
	std::basic_string_view<T> StringViewOf(const std::basic_string_view<T>& view) noexcept
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

	template<class T>
	const auto ScopedCharBufferOf(T&& value) noexcept
	{
		auto view = StringViewOf(std::forward<T>(value));
		using CharType = typename decltype(view)::value_type;

		return wxScopedCharTypeBuffer<CharType>::CreateNonOwned(view.data(), view.length());
	}
}

namespace KxFramework
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

		public:
			static int Compare(std::string_view left, std::string_view right, StringOpFlag flags = StringOpFlag::None) noexcept;
			static int Compare(std::wstring_view left, std::wstring_view right, StringOpFlag flags = StringOpFlag::None) noexcept;
			static int Compare(const wxString& left, const wxString& right, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Compare(StringViewOf(left), StringViewOf(right), flags);
			}
			static int Compare(const String& left, const String& right, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Compare(left.GetView(), right.GetView(), flags);
			}
			static int Compare(const char* left, const char* right, StringOpFlag flags = StringOpFlag::None)
			{
				return Compare(StringViewOf(left), StringViewOf(right), flags);
			}
			static int Compare(const wchar_t* left, const wchar_t* right, StringOpFlag flags = StringOpFlag::None)
			{
				return Compare(StringViewOf(left), StringViewOf(right), flags);
			}
			
			static int Compare(wxUniChar left, wxUniChar right, StringOpFlag flags = StringOpFlag::None) noexcept;
			static int Compare(wxUniCharRef left, wxUniCharRef right, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Compare(wxUniChar(left), wxUniChar(right), flags);
			}
			static int Compare(char left, char right, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Compare(wxUniChar(left), wxUniChar(right), flags);
			}
			static int Compare(wchar_t left, wchar_t right, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Compare(wxUniChar(left), wxUniChar(right), flags);
			}

			static bool Matches(std::string_view name, std::string_view expression, StringOpFlag flags = StringOpFlag::None) noexcept;
			static bool Matches(std::wstring_view name, std::wstring_view expression, StringOpFlag flags = StringOpFlag::None) noexcept;
			static bool Matches(const wxString& name, const wxString& expression, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Matches(StringViewOf(name), StringViewOf(expression), flags);
			}
			static bool Matches(const String& name, const String& expression, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Matches(name.GetView(), expression.GetView(), flags);
			}
			static bool Matches(const char* name, const char* expression, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Matches(std::string_view(name), std::string_view(expression), flags);
			}
			static bool Matches(const wchar_t* name, const wchar_t* expression, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Matches(std::wstring_view(name), std::wstring_view(expression), flags);
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
				return wxString::FromAscii(ascii, npos);
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

			static wxUniChar ToLower(wxUniChar c) noexcept;
			static wxUniChar ToUpper(wxUniChar c) noexcept;

			template<class... Args>
			static String Concat(Args&&... arg)
			{
				return (String(arg) + ...);
			}

			template<class... Args>
			static String ConcatWithSeparator(const String& sep, Args&&... arg)
			{
				String value = ((String(std::forward<Args>(arg)) + sep) + ...);
				value.RemoveFromEnd(sep.length());
				return value;
			}

			template<class TFunc>
			static size_t SplitBySeparator(const String& string, const String& sep, TFunc&& func, StringOpFlag flags = StringOpFlag::None)
			{
				if (sep.empty() && !string.empty())
				{
					std::invoke(func, string.GetView());
					return 1;
				}

				size_t separatorPos = string.Find(sep, 0, flags);
				if (separatorPos == String::npos)
				{
					std::invoke(func, string.GetView());
					return 1;
				}

				size_t pos = 0;
				size_t count = 0;
				while (pos < string.length() && separatorPos <= string.length())
				{
					StringView stringPiece = string.GetView().substr(pos, separatorPos - pos);
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
			static size_t SplitByLength(StringView string, size_t length, TFunc&& func)
			{
				if (length != 0)
				{
					const size_t stringLength = string.length();

					size_t count = 0;
					for (size_t i = 0; i < stringLength; i += length)
					{
						StringView stringPiece = string.substr(i, length);
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
					std::invoke(func, string);
					return 1;
				}
				return 0;
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
				:m_String(data, length)
			{
			}
			String(const char* data, const wxMBConv& conv, size_t length = npos)
				:m_String(data, conv, length)
			{
			}
			String(const wchar_t* data, size_t length = npos)
				:m_String(data, length)
			{
			}
			
			template<class T, size_t size>
			String(const T(&data)[size])
				:m_String(data, size - 1)
			{
			}
			
			template<size_t size>
			String(const char(&data)[size], const wxMBConv& conv)
				:m_String(data, conv, size - 1)
			{
			}

			// std::[w]string_view
			String(std::string_view other)
				:m_String(other.data(), other.length())
			{
			}
			String(std::string_view other, const wxMBConv& conv)
				:m_String(other.data(), conv, other.length())
			{
			}
			String(std::wstring_view other)
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
			const char* c_str() const noexcept(std::is_same_v<XChar, char>)
			{
				return m_String.c_str();
			}
			const wchar_t* wc_str() const noexcept(std::is_same_v<XChar, wchar_t>)
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
			String& Append(std::string_view other)
			{
				m_String.Append(other.data(), other.length());
				return *this;
			}
			String& Append(std::wstring_view other)
			{
				m_String.Append(other.data(), other.length());
				return *this;
			}
			String& Append(const String& other)
			{
				m_String.Append(other.m_String);
				return *this;
			}
			String& Append(const wxString& other)
			{
				m_String.Append(other);
				return *this;
			}
			String& Append(const char* other)
			{
				m_String.Append(other);
				return *this;
			}
			String& Append(const wchar_t* other)
			{
				m_String.Append(other);
				return *this;
			}
			
			template<class T, size_t size>
			String& Append(const T(&other)[size])
			{
				m_String.Append(other, size - 1);
				return *this;
			}

			String& Append(wxUniChar c, size_t count = 1)
			{
				m_String.Append(c, count);
				return *this;
			}
			String& Append(wxUniCharRef c, size_t count = 1)
			{
				m_String.Append(c, count);
				return *this;
			}
			String& Append(char c, size_t count = 1)
			{
				m_String.Append(c, count);
				return *this;
			}
			String& Append(wchar_t c, size_t count = 1)
			{
				m_String.Append(c, count);
				return *this;
			}

			String& Prepend(std::string_view other)
			{
				m_String.insert(0, other.data(), other.length());
				return *this;
			}
			String& Prepend(std::wstring_view other)
			{
				m_String.insert(0, other.data(), other.length());
				return *this;
			}
			String& Prepend(const String& other)
			{
				m_String.insert(0, other.m_String);
				return *this;
			}
			String& Prepend(const wxString& other)
			{
				m_String.insert(0, other);
				return *this;
			}
			String& Prepend(const char* other)
			{
				m_String.insert(0, other);
				return *this;
			}
			String& Prepend(const wchar_t* other)
			{
				m_String.insert(0, other);
				return *this;
			}
			
			template<class T, size_t size>
			String& Prepend(const T(&other)[size])
			{
				m_String.insert(0, other, size - 1);
				return *this;
			}

			String& Prepend(wxUniChar c, size_t count = 1)
			{
				m_String.insert(0, count, c);
				return *this;
			}
			String& Prepend(wxUniCharRef c, size_t count = 1)
			{
				m_String.insert(0, count, c);
				return *this;
			}
			String& Prepend(char c, size_t count = 1)
			{
				m_String.insert(0, count, c);
				return *this;
			}
			String& Prepend(wchar_t c, size_t count = 1)
			{
				m_String.insert(0, count, c);
				return *this;
			}

			String& Insert(size_t pos, std::string_view other)
			{
				m_String.insert(pos, other.data(), other.length());
				return *this;
			}
			String& Insert(size_t pos, std::wstring_view other)
			{
				m_String.insert(pos, other.data(), other.length());
				return *this;
			}
			String& Insert(size_t pos, const String& other)
			{
				m_String.insert(pos, other.m_String);
				return *this;
			}
			String& Insert(size_t pos, const wxString& other)
			{
				m_String.insert(pos, other);
				return *this;
			}
			String& Insert(size_t pos, const char* other)
			{
				m_String.insert(pos, other);
				return *this;
			}
			String& Insert(size_t pos, const wchar_t* other)
			{
				m_String.insert(pos, other);
				return *this;
			}
			
			template<class T, size_t size>
			String& Insert(size_t pos, const T(&other)[size])
			{
				m_String.insert(pos, other, size - 1);
				return *this;
			}

			String& Insert(size_t pos, wxUniChar c, size_t count = 1)
			{
				m_String.insert(pos, count, c);
				return *this;
			}
			String& Insert(size_t pos, wxUniCharRef c, size_t count = 1)
			{
				m_String.insert(pos, count, c);
				return *this;
			}
			String& Insert(size_t pos, char c, size_t count = 1)
			{
				m_String.insert(pos, count, c);
				return *this;
			}
			String& Insert(size_t pos, wchar_t c, size_t count = 1)
			{
				m_String.insert(pos, count, c);
				return *this;
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

			// Comparison
			int CompareTo(std::string_view other, StringOpFlag flags = StringOpFlag::None) const noexcept(std::is_same_v<XChar, char>)
			{
				#if wxUSE_UNICODE_WCHAR
				return Compare(*this, other, flags);
				#else
				return Compare(GetView(), other, flags);
				#endif
			}
			int CompareTo(std::wstring_view other, StringOpFlag flags = StringOpFlag::None) const noexcept(std::is_same_v<XChar, wchar_t>)
			{
				#if wxUSE_UNICODE_WCHAR
				return Compare(GetView(), other, flags);
				#else
				return Compare(*this, other, flags);
				#endif
			}
			int CompareTo(const wxString& other, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				return CompareTo(StringViewOf(other), flags);
			}
			int CompareTo(const String& other, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				return CompareTo(StringViewOf(other), flags);
			}
			int CompareTo(const char* other, StringOpFlag flags = StringOpFlag::None) const
			{
				return CompareTo(StringViewOf(other), flags);
			}
			int CompareTo(const wchar_t* other, StringOpFlag flags = StringOpFlag::None) const
			{
				return CompareTo(StringViewOf(other), flags);
			}
			
			int CompareTo(wxUniChar other, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				const XChar c[2] = {other, 0};
				return Compare(GetView(), c, flags);
			}
			int CompareTo(wxUniCharRef other, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				return CompareTo(wxUniChar(other), flags);
			}
			int CompareTo(char other, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				return CompareTo(wxUniChar(other), flags);
			}
			int CompareTo(wchar_t other, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				return CompareTo(wxUniChar(other), flags);
			}

			template<class T>
			bool IsSameAs(T&& other, StringOpFlag flags = StringOpFlag::None) const
			{
				return CompareTo(std::forward<T>(other), flags) == 0;
			}

			bool StartsWith(std::string_view pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const;
			bool StartsWith(std::wstring_view pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const;
			bool StartsWith(const wxString& pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const
			{
				return StartsWith(StringViewOf(pattern), rest, flags);
			}
			bool StartsWith(const String& pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const
			{
				return StartsWith(StringViewOf(pattern), rest, flags);
			}
			bool StartsWith(const char* pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const
			{
				return StartsWith(StringViewOf(pattern), rest, flags);
			}
			bool StartsWith(const wchar_t* pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const
			{
				return StartsWith(StringViewOf(pattern), rest, flags);
			}
			
			bool EndsWith(std::string_view pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const;
			bool EndsWith(std::wstring_view pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const;
			bool EndsWith(const wxString& pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const
			{
				return EndsWith(StringViewOf(pattern), rest, flags);
			}
			bool EndsWith(const String& pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const
			{
				return EndsWith(StringViewOf(pattern), rest, flags);
			}
			bool EndsWith(const char* pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const
			{
				return EndsWith(StringViewOf(pattern), rest, flags);
			}
			bool EndsWith(const wchar_t* pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const
			{
				return EndsWith(StringViewOf(pattern), rest, flags);
			}

			bool Matches(std::string_view expression, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				#if wxUSE_UNICODE_WCHAR
				return Matches(*this, expression, flags);
				#else
				return Matches(GetView(), expression, flags);
				#endif

				return Matches(GetView(), expression, flags);
			}
			bool Matches(std::wstring_view expression, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				#if wxUSE_UNICODE_WCHAR
				return Matches(GetView(), expression, flags);
				#else
				return Matches(*this, expression, flags);
				#endif
			}
			bool Matches(const wxString& expression, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Matches(StringViewOf(expression), flags);
			}
			bool Matches(const String& expression, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Matches(StringViewOf(expression), flags);
			}
			bool Matches(const char* expression, StringOpFlag flags = StringOpFlag::None)
			{
				return Matches(StringViewOf(expression), flags);
			}
			bool Matches(const wchar_t* expression, StringOpFlag flags = StringOpFlag::None)
			{
				return Matches(StringViewOf(expression), flags);
			}

			// Substring extraction
			String Mid(size_t offset, size_t count = wxString::npos) const
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

			String AfterFirst(wxUniChar c, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const;
			String AfterLast(wxUniChar c, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const;

			String BeforeFirst(wxUniChar c, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const;
			String BeforeLast(wxUniChar c, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const;

			template<class TFunc>
			size_t SplitBySeparator(const String& sep, TFunc&& func, StringOpFlag flags = StringOpFlag::None) const
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
			size_t Find(std::string_view pattern, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) const;
			size_t Find(std::wstring_view pattern, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) const;
			size_t Find(const wxString& pattern, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) const
			{
				return Find(StringViewOf(pattern), offset, flags);
			}
			size_t Find(const String& pattern, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) const
			{
				return Find(StringViewOf(pattern), offset, flags);
			}
			
			size_t Find(wxUniChar pattern, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) const noexcept;
			size_t Find(wxUniCharRef pattern, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				return Find(wxUniChar(pattern), offset, flags);
			}
			size_t Find(char pattern, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				return Find(wxUniChar(pattern), offset, flags);
			}
			size_t Find(wchar_t pattern, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				return Find(wxUniChar(pattern), offset, flags);
			}

			size_t Replace(std::string_view pattern, std::string_view replacement, size_t offset = 0, StringOpFlag flags = StringOpFlag::None);
			size_t Replace(std::wstring_view pattern, std::wstring_view replacement, size_t offset = 0, StringOpFlag flags = StringOpFlag::None);
			size_t Replace(const wxString& pattern, const wxString& replacement, size_t offset = 0, StringOpFlag flags = StringOpFlag::None)
			{
				return Replace(StringViewOf(pattern), StringViewOf(replacement), offset, flags);
			}
			size_t Replace(const String& pattern, const String& replacement, size_t offset = 0, StringOpFlag flags = StringOpFlag::None)
			{
				return Replace(StringViewOf(pattern), StringViewOf(replacement), offset, flags);
			}
			
			size_t Replace(wxUniChar c, wxUniChar replacement, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) noexcept;
			size_t Replace(wxUniCharRef c, wxUniChar replacement, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Replace(wxUniChar(c), wxUniChar(replacement), offset, flags);
			}
			size_t Replace(char c, wxUniChar replacement, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Replace(wxUniChar(c), wxUniChar(replacement), offset, flags);
			}
			size_t Replace(wchar_t c, wxUniChar replacement, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Replace(wxUniChar(c), wxUniChar(replacement), offset, flags);
			}

			template<class T>
			bool Contains(T&& pattern, StringOpFlag flags = StringOpFlag::None) const
			{
				return Find(std::forward<T>(pattern), 0, flags) == npos;
			}

			// Conversion to numbers
			bool ToFloat(float& value) const noexcept
			{
				double dValue = value;
				if (m_String.ToDouble(&dValue))
				{
					value = static_cast<float>(dValue);
					return true;
				}
				return false;
			}
			bool ToCFloat(float& value) const noexcept
			{
				double dValue = value;
				if (m_String.ToCDouble(&dValue))
				{
					value = static_cast<float>(dValue);
					return true;
				}
				return false;
			}

			bool ToDouble(double& value) const noexcept
			{
				return m_String.ToDouble(&value);
			}
			bool ToCDouble(double& value) const noexcept
			{
				return m_String.ToCDouble(&value);
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
			String& Trim(StringOpFlag flags = StringOpFlag::None)
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
	};
}

namespace KxFramework
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
	struct hash<KxFramework::String>
	{
		size_t operator()(const KxFramework::String& string) const noexcept
		{
			return std::hash<KxFramework::StringView>()(string.GetView());
		}
	};
}
