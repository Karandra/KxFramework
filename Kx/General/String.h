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

	enum class StrCmpFlag
	{
		None = 0,
		IgnoreCase = 1 << 0
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowEverything(StrCmpFlag);
	}
}

namespace KxFramework
{
	class KX_API String final
	{
		public:
			static int Compare(StringView left, StringView right, StrCmpFlag flags = StrCmpFlag::None) noexcept;

		private:
			wxString m_String;

		public:
			String() noexcept = default;
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
			String(const char* data, size_t length)
				:m_String(data, length)
			{
			}
			String(const wchar_t* data, size_t length)
				:m_String(data, length)
			{
			}
			~String() noexcept = default;

		public:
			bool IsEmpty() const noexcept
			{
				return m_String.IsEmpty();
			}
			size_t GetLength() const noexcept
			{
				return m_String.length();
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
			StringView GetView() const noexcept
			{
				return StringView(m_String.wx_str(), m_String.length());
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

			std::string ToStdString(const wxMBConv& conv = wxConvLibc) const
			{
				return m_String.ToStdString(conv);
			}
			std::wstring ToStdWString() const
			{
				return m_String.ToStdWstring();
			}

			int CompareTo(StringView other, StrCmpFlag flags = StrCmpFlag::None) const noexcept
			{
				return Compare(*this, other, flags);
			}
			bool IsSameAs(StringView other, StrCmpFlag flags = StrCmpFlag::None) const noexcept
			{
				return CompareTo(other, flags) == 0;
			}

		public:
			String& operator=(const String&) = default;
			String& operator=(String&& other) noexcept(Private::IsWxStringMoveable())
			{
				Private::MoveWxString(m_String, other.m_String);
				return *this;
			}
			String& operator=(wxString&& other) noexcept(Private::IsWxStringMoveable())
			{
				Private::MoveWxString(m_String, other);
				return *this;
			}

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

			template<class T>
			bool operator!=(const T& other) const noexcept
			{
				return !(*this == other);
			}

			operator wxString&() noexcept
			{
				return m_String;
			}
			operator const wxString&() const noexcept
			{
				return m_String;
			}
			operator StringView() const noexcept
			{
				return GetView();
			}
	};
}
