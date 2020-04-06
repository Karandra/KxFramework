#pragma once
#include "Common.h"
#include "Private/String.h"
#include <string_view>
#include <wx/string.h>
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
	class KX_API String final
	{
		public:
			using iterator = wxString::iterator;
			using const_iterator = wxString::const_iterator;
			using reverse_iterator = wxString::reverse_iterator;
			using const_reverse_iterator = wxString::const_reverse_iterator;

			using traits_type = std::char_traits<XChar>;
			using allocator_type = std::allocator<XChar>;

		public:
			static constexpr size_t npos = StringView::npos;

		public:
			static int Compare(const wxString& left, const wxString& right, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Compare(StringView(left.wx_str(), left.length()), StringView(right.wx_str(), right.length()), flags);
			}
			static int Compare(const String& left, const String& right, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Compare(left.wx_view(), right.wx_view(), flags);
			}
			static int Compare(StringView left, StringView right, StringOpFlag flags = StringOpFlag::None) noexcept;
			static int Compare(wxUniChar left, wxUniChar right, StringOpFlag flags = StringOpFlag::None) noexcept;

			static bool Matches(const wxString& name, const wxString& expression, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Matches(StringView(name.wx_str(), name.length()), StringView(expression.wx_str(), expression.length()), flags);
			}
			static bool Matches(const String& name, const String& expression, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Matches(name.wx_view(), expression.wx_view(), flags);
			}
			static bool Matches(std::string_view name, std::string_view expression, StringOpFlag flags = StringOpFlag::None) noexcept;
			static bool Matches(std::wstring_view name, std::wstring_view expression, StringOpFlag flags = StringOpFlag::None) noexcept;
			
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
				return wxString::FromUTF8(utf8, npos);
			}
			static String FromUTF8(std::string_view utf8)
			{
				return wxString::FromUTF8(utf8.data(), utf8.length());
			}
			static String FromUTF8(const wxScopedCharBuffer& buffer)
			{
				return wxString::FromUTF8(buffer.data(), buffer.length());
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
			static String FromASCII(const wxScopedCharBuffer& buffer)
			{
				return wxString::FromAscii(buffer.data(), buffer.length());
			}

			static String From8BitData(const char* data, size_t length = npos)
			{
				if (length != npos)
				{
					wxString::From8BitData(data, npos);
				}
				return wxString::From8BitData(data);
			}
			static String From8BitData(const wxScopedCharBuffer& buffer)
			{
				return wxString::From8BitData(buffer.data(), buffer.length());
			}

			static wxUniChar ToLower(const wxUniChar& c) noexcept;
			static wxUniChar ToUpper(const wxUniChar& c) noexcept;

		private:
			wxString m_String;

		public:
			String() = default;
			String(const String&) = default;
			String(const wxString& other)
				:m_String(other)
			{
			}
			String(String&& other) noexcept(Private::IsWxStringMoveable())
			{
				*this = std::move(other);
			}
			String(wxString&& other) noexcept(Private::IsWxStringMoveable())
			{
				*this = std::move(other);
			}
			String(StringView other)
				:m_String(other.data(), other.length())
			{
			}
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
			String(const std::string& other)
				:m_String(other)
			{
			}
			String(const std::wstring& other)
				:m_String(other)
			{
			}
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
			StringView wx_view() const noexcept
			{
				return StringView(m_String.wx_str(), m_String.length());
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
			String& Append(StringView other)
			{
				m_String.Append(other.data(), other.length());
				return *this;
			}
			String& Append(wxUniChar c, size_t count = 1)
			{
				m_String.Append(c, count);
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
			String& Prepend(StringView other)
			{
				m_String.insert(0, other.data(), other.length());
				return *this;
			}
			String& Prepend(wxUniChar c, size_t count = 1)
			{
				m_String.insert(0, c, count);
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

			template<class T>
			String& operator+=(const T& other)
			{
				m_String.Append(other);
				return *this;
			}

			template<class T>
			String& operator<<(const T& other)
			{
				m_String.Append(other);
				return *this;
			}

			// Comparison
			int CompareTo(const wxString& other, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				return Compare(*this, StringView(other.wx_str(), other.length()), flags);
			}
			int CompareTo(const String& other, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				return Compare(wx_view(), other, flags);
			}
			int CompareTo(StringView other, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				return Compare(*this, other, flags);
			}
			int CompareTo(wxUniChar other, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				const XChar c[2] = {other, 0};
				return Compare(*this, c, flags);
			}

			bool IsSameAs(const wxString& other, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				return CompareTo(other, flags) == 0;
			}
			bool IsSameAs(const String& other, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				return CompareTo(other, flags) == 0;
			}
			bool IsSameAs(StringView other, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				return CompareTo(other, flags) == 0;
			}
			bool IsSameAs(wxUniChar other, StringOpFlag flags = StringOpFlag::None) const noexcept
			{
				return CompareTo(other, flags) == 0;
			}

			bool StartsWith(const wxString& pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const
			{
				return StartsWith(StringView(pattern.wx_str(), pattern.length()), rest, flags);
			}
			bool StartsWith(const String& pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const
			{
				return StartsWith(pattern.wx_view(), rest, flags);
			}
			bool StartsWith(StringView pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const;
			
			bool EndsWith(const wxString& pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const
			{
				return EndsWith(StringView(pattern.wx_str(), pattern.length()), rest, flags);
			}
			bool EndsWith(const String& pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const
			{
				return EndsWith(pattern.wx_view(), rest, flags);
			}
			bool EndsWith(StringView pattern, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const;

			bool Matches(const wxString& expression, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Matches(wx_view(), StringView(expression.wx_str(), expression.length()), flags);
			}
			bool Matches(const String& expression, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Matches(wx_view(), expression, flags);
			}
			bool Matches(StringView expression, StringOpFlag flags = StringOpFlag::None) noexcept
			{
				return Matches(wx_view(), expression, flags);
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

			String AfterFirst(wxUniChar c, StringOpFlag flags = StringOpFlag::None) const;
			String AfterLast(wxUniChar c, StringOpFlag flags = StringOpFlag::None) const;

			String BeforeFirst(wxUniChar c, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const;
			String BeforeLast(wxUniChar c, String* rest = nullptr, StringOpFlag flags = StringOpFlag::None) const;

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
				if (!m_String.IsEmpty())
				{
					return Clone().MakeCapitalized();
				}
				return {};
			}

			// Searching and replacing
			size_t Find(const wxString& pattern, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) const
			{
				return Find(StringView(pattern.wx_str(), pattern.length()), offset, flags);
			}
			size_t Find(const String& pattern, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) const
			{
				return Find(pattern.wx_view(), offset, flags);
			}
			size_t Find(StringView pattern, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) const;
			size_t Find(wxUniChar c, size_t offset = 0, StringOpFlag flags = StringOpFlag::None) const;

			size_t Replace(const wxString& pattern, const wxString& replacement, size_t offset = 0, StringOpFlag flags = StringOpFlag::None)
			{
				return Replace(StringView(pattern.wx_str(), pattern.length()), StringView(replacement.wx_str(), replacement.length()), offset, flags);
			}
			size_t Replace(const String& pattern, const String& replacement, size_t offset = 0, StringOpFlag flags = StringOpFlag::None)
			{
				return Replace(pattern.wx_view(), pattern.wx_view(), offset, flags);
			}
			size_t Replace(StringView pattern, StringView replacement, size_t offset = 0, StringOpFlag flags = StringOpFlag::None);
			size_t Replace(wxUniChar pattern, wxUniChar replacement, size_t offset = 0, StringOpFlag flags = StringOpFlag::None);

			bool Contains(const wxString& pattern, StringOpFlag flags = StringOpFlag::None) const
			{
				return Find(pattern, 0, flags) != npos;
			}
			bool Contains(const String& pattern, StringOpFlag flags = StringOpFlag::None) const
			{
				return Find(pattern, 0, flags) != npos;
			}
			bool Contains(const StringView& pattern, StringOpFlag flags = StringOpFlag::None) const
			{
				return Find(pattern, 0, flags) != npos;
			}
			bool Contains(wxUniChar c, StringOpFlag flags = StringOpFlag::None) const
			{
				return Find(c, 0, flags) != npos;
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
			bool empty() noexcept
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

			wxUniChar front(size_t i) const
			{
				return m_String[0];
			}
			wxUniCharRef front(size_t i)
			{
				return m_String[0];
			}

			wxUniChar back(size_t i) const
			{
				return m_String[m_String.length() - 1];
			}
			wxUniCharRef back(size_t i)
			{
				return m_String[m_String.length() - 1];
			}

		public:
			// Assignment
			String& operator=(const String&) = default;
			String& operator=(const wxString& other)
			{
				m_String = other;
				return *this;
			}
			String& operator=(StringView other)
			{
				m_String.assign(other.data(), other.length());
				return *this;
			}
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

			template<class T>
			String& operator=(T&& other)
			{
				m_String = std::forward<T>(other);
				return *this;
			}

			// Comparison
			template<class T>
			bool operator==(const T& other) const noexcept
			{
				if constexpr(std::is_same_v<T, String>)
				{
					return m_String == other.m_String;
				}
				else
				{
					return m_String == other;
				}
			}

			bool operator==(StringView other) const noexcept
			{
				return wx_view() == other;
			}

			template<class T>
			bool operator!=(const T& other) const noexcept
			{
				return !(*this == other);
			}

			// Conversion
			operator StringView() const noexcept
			{
				return wx_view();
			}
			operator wxString&() noexcept
			{
				return GetWxString();
			}
			operator const wxString&() const noexcept
			{
				return GetWxString();
			}
	};
}

namespace std
{
	template<>
	struct hash<KxFramework::String>
	{
		size_t operator()(const KxFramework::String& string) const noexcept
		{
			return std::hash<KxFramework::StringView>()(string.wx_view());
		}
	};
}
