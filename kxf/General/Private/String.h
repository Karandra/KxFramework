#pragma once
#include "kxf/wxWidgets/Setup.h"
#include <wx/string.h>
#include <string>

namespace kxf
{
	class String;
}

namespace kxf::Private
{
	#define Kx_WxStringConvertibleToStd	wxUSE_STL_BASED_WXSTRING && !wxUSE_UNICODE_UTF8

	inline constexpr bool IsWxStringConvertibleToStd() noexcept
	{
		#if Kx_WxStringConvertibleToStd
		return true;
		#else
		return false;
		#endif
	}
	inline constexpr bool IsWxStringMoveable() noexcept
	{
		return IsWxStringConvertibleToStd();
	}

	#if Kx_WxStringConvertibleToStd
	inline const wxStringImpl& GetWxStringImpl(const wxString& string) noexcept
	{
		#if wxUSE_UNICODE_WCHAR
		return string.ToStdWstring();
		#else
		return string.ToStdString();
		#endif
	}
	inline wxStringImpl& GetWxStringImpl(wxString& string) noexcept
	{
		#if wxUSE_UNICODE_WCHAR
		return const_cast<wxStringImpl&>(string.ToStdWstring());
		#else
		return const_cast<wxStringImpl&>(string.ToStdString());
		#endif
	}
	#endif

	inline void MoveWxString(wxString& destination, wxString&& source) noexcept(IsWxStringConvertibleToStd())
	{
		if (&source != &destination)
		{
			#if Kx_WxStringConvertibleToStd
			{
				// Also see a comment in the next overload
				GetWxStringImpl(destination) = std::move(GetWxStringImpl(source));
			}
			#else
			{
				destination = std::move(source);
				source.clear();
			}
			#endif
		}
	}
	inline void MoveWxString(wxString& destination, wxStringImpl&& source) noexcept(IsWxStringConvertibleToStd())
	{
		#if Kx_WxStringConvertibleToStd
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
		#else
		{
			destination = std::move(source);
			source.clear();
		}
		#endif
	}

	template<class T>
	constexpr bool IsAnyCharType() noexcept
	{
		using Tx = std::remove_const_t<std::remove_reference_t<T>>;
		return !std::is_array_v<T> && !std::is_pointer_v<T> && (std::is_same_v<Tx, char> || std::is_same_v<Tx, wchar_t> || std::is_same_v<Tx, wxUniChar> || std::is_same_v<Tx, wxUniCharRef>);
	}

	template<class T>
	constexpr bool IsAnyStringType() noexcept
	{
		return !IsAnyCharType<T>();
	}
}
