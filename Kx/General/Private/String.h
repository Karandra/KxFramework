#pragma once
#include <wx/string.h>

namespace KxFramework
{
	class String;
}

namespace KxFramework::Private
{
	#define Kx_WxStringConvertibleToStd	wxUSE_STL_BASED_WXSTRING && !wxUSE_UNICODE_UTF8

	constexpr bool IsWxStringConvertibleToStd() noexcept
	{
		#if Kx_WxStringConvertibleToStd
		return true;
		#else
		return false;
		#endif
	}
	constexpr bool IsWxStringMoveable() noexcept
	{
		return IsWxStringConvertibleToStd();
	}

	#if Kx_WxStringConvertibleToStd
	const wxStringImpl& GetWxStringImpl(const wxString& string) noexcept
	{
		#if wxUSE_UNICODE_WCHAR
		return string.ToStdWstring();
		#else
		return string.ToStdString();
		#endif
	}
	wxStringImpl& GetWxStringImpl(wxString& string) noexcept
	{
		#if wxUSE_UNICODE_WCHAR
		return const_cast<wxStringImpl&>(string.ToStdWstring());
		#else
		return const_cast<wxStringImpl&>(string.ToStdString());
		#endif
	}
	#endif

	void MoveWxString(wxString& destination, wxString&& source) noexcept(IsWxStringConvertibleToStd())
	{
		if (&source != &destination)
		{
			#if Kx_WxStringConvertibleToStd
			{
				GetWxStringImpl(destination) = std::move(GetWxStringImpl(source));

				// wxString contains an extra buffer (m_convertedTo[W]Char) to hold converted string
				// returned by 'wxString::AsCharBuf' but it seems it can be left untouched since wxString
				// always rewrites its content when requested to make conversion and only changes its size
				// when needed.
			}
			#else
			{
				destination = std::move(source);
				source.clear();
			}
			#endif
		}
	}

	using ViewOrWxStringW = std::conditional_t<wxUSE_UNICODE_WCHAR, std::basic_string_view<wchar_t>, wxString>;
	using ViewOrWxStringA = std::conditional_t<wxUSE_UNICODE_WCHAR, wxString, std::basic_string_view<char>>;
}
