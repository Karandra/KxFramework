#pragma once
#include "Common.h"
#include <format>
#include <string>
#include <string_view>
#include <wx/string.h>

namespace std
{
	// wxString
	template<>
	struct formatter<wxString, char>: std::formatter<std::string_view, char>
	{
		template<class TFormatContext>
		auto format(const wxString& value, TFormatContext& formatContext) const
		{
			auto utf8 = value.ToUTF8();
			return std::formatter<std::string_view, char>::format(std::string_view(utf8.data(), utf8.length()), formatContext);
		}
	};

	template<>
	struct formatter<wxString, wchar_t>: std::formatter<std::wstring_view, wchar_t>
	{
		template<class TFormatContext>
		auto format(const wxString& value, TFormatContext& formatContext) const
		{
			return std::formatter<std::wstring_view, wchar_t>::format(std::wstring_view(value.wc_str(), value.length()), formatContext);
		}
	};
}
