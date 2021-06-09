#pragma once
#include "Common.h"
#include "String.h"
#include <format>
#include <string>
#include <string_view>

namespace kxf::Private
{
	String ConvertQtStyleFormat(const String& format);
	void LogFormatterException(const std::format_error& e);

	template<class... Args>
	String DoFromat(StringView format, Args&&... arg)
	{
		if constexpr((sizeof...(Args)) == 0)
		{
			return String::FromView(format);
		}

		try
		{
			return std::format(format, std::forward<Args>(arg)...);
		}
		catch (const std::format_error& e)
		{
			LogFormatterException(e);
			return String::FromView(format);
		}
		return {};
	}

	template<class... Args>
	size_t DoFormattedSize(StringView format, Args&&... arg)
	{
		if constexpr((sizeof...(Args)) == 0)
		{
			return format.length();
		}

		try
		{
			return std::formatted_size(format, std::forward<Args>(arg)...);
		}
		catch (const std::format_error& e)
		{
			LogFormatterException(e);
			return 0;
		}
		return 0;
	}
}

namespace kxf
{
	template<class... Args>
	String Format(const char* format, Args&&... arg)
	{
		if (format)
		{
			auto utf8 = String::FromUTF8(format);
			return Private::DoFromat(utf8.GetView(), std::forward<Args>(arg)...);
		}
		return {};
	}

	template<class... Args>
	String Format(const wchar_t* format, Args&&... arg)
	{
		if (format)
		{
			return Private::DoFromat(format, std::forward<Args>(arg)...);
		}
		return {};
	}

	template<class... Args>
	String Format(std::string_view format, Args&&... arg)
	{
		auto utf8 = String::FromUTF8(format);
		return Private::DoFromat(utf8.GetView(), std::forward<Args>(arg)...);
	}

	template<class... Args>
	String Format(std::wstring_view format, Args&&... arg)
	{
		return Private::DoFromat(format, std::forward<Args>(arg)...);
	}

	template<class... Args>
	String Format(const String& format, Args&&... arg)
	{
		return Private::DoFromat(format.GetView(), std::forward<Args>(arg)...);
	}

	template<class... Args>
	String FormatQtStyle(const String& format, Args&&... arg)
	{
		return Format(Private::ConvertQtStyleFormat(format), std::forward<Args>(arg)...);
	}
}

namespace kxf
{
	template<class... Args>
	size_t FormattedSize(const char* format, Args&&... arg)
	{
		if (format)
		{
			auto utf8 = String::FromUTF8(format);
			return Private::DoFormattedSize(utf8.GetView(), std::forward<Args>(arg)...);
		}
		return 0;
	}

	template<class... Args>
	size_t FormattedSize(const wchar_t* format, Args&&... arg)
	{
		if (format)
		{
			return Private::DoFormattedSize(format, std::forward<Args>(arg)...);
		}
		return 0;
	}

	template<class... Args>
	size_t FormattedSize(std::string_view format, Args&&... arg)
	{
		auto utf8 = String::FromUTF8(format);
		return Private::DoFormattedSize(utf8.GetView(), std::forward<Args>(arg)...);
	}

	template<class... Args>
	size_t FormattedSize(std::wstring_view format, Args&&... arg)
	{
		return Private::DoFormattedSize(format, std::forward<Args>(arg)...);
	}

	template<class... Args>
	size_t FormattedSize(const String& format, Args&&... arg)
	{
		return Private::DoFormattedSize(format.GetView(), std::forward<Args>(arg)...);
	}
}

namespace std
{
	// kxf::String
	template<>
	struct formatter<kxf::String, char>: std::formatter<std::string_view, char>
	{
		template<class TFormatContext>
		auto format(const kxf::String& value, TFormatContext& formatContext)
		{
			auto utf8 = value.ToUTF8();
			return std::formatter<std::string_view, char>::format(utf8, formatContext);
		}
	};

	template<>
	struct formatter<kxf::String, wchar_t>: std::formatter<std::wstring_view, wchar_t>
	{
		template<class TFormatContext>
		auto format(const kxf::String& value, TFormatContext& formatContext)
		{
			return std::formatter<std::wstring_view, wchar_t>::format(value.GetView(), formatContext);
		}
	};

	// wxString
	template<>
	struct formatter<wxString, char>: std::formatter<std::string_view, char>
	{
		template<class TFormatContext>
		auto format(const wxString& value, TFormatContext& formatContext)
		{
			auto utf8 = value.ToUTF8();
			return std::formatter<std::string_view, char>::format(std::string_view(utf8.data(), utf8.length()), formatContext);
		}
	};

	template<>
	struct formatter<wxString, wchar_t>: std::formatter<std::wstring_view, wchar_t>
	{
		template<class TFormatContext>
		auto format(const wxString& value, TFormatContext& formatContext)
		{
			return std::formatter<std::wstring_view, wchar_t>::format(std::wstring_view(value.wx_str(), value.length()), formatContext);
		}
	};

	// Converting const char[N]/wchar_t[N]
	template<size_t N>
	struct formatter<char[N], wchar_t>: std::formatter<std::wstring_view, wchar_t>
	{
		template<class T, class TFormatContext>
		auto format(const T& value, TFormatContext& formatContext)
		{
			auto converted = kxf::String::FromUTF8(value, N != 0 ? N - 1 : 0);
			return std::formatter<std::wstring_view, wchar_t>::format(converted.GetView(), formatContext);
		}
	};

	template<size_t N>
	struct formatter<wchar_t[N], char>: std::formatter<std::string_view, char>
	{
		template<class T, class TFormatContext>
		auto format(const T& value, TFormatContext& formatContext)
		{
			auto utf8 = kxf::String(value, N != 0 ? N - 1 : 0).ToUTF8();
			return std::formatter<std::string_view, char>::format(utf8, formatContext);
		}
	};

	// Converting const char*/wchar_t*
	template<>
	struct formatter<const char*, wchar_t>: std::formatter<std::wstring_view, wchar_t>
	{
		template<class TFormatContext>
		auto format(const char* value, TFormatContext& formatContext)
		{
			auto converted = kxf::String::FromUTF8(value);
			return std::formatter<std::wstring_view, wchar_t>::format(converted.GetView(), formatContext);
		}
	};

	template<>
	struct formatter<const wchar_t*, char>: std::formatter<std::string_view, char>
	{
		template<class TFormatContext>
		auto format(const wchar_t* value, TFormatContext& formatContext)
		{
			auto utf8 = kxf::String(value).ToUTF8();
			return std::formatter<std::string_view, char>::format(utf8, formatContext);
		}
	};

	// Converting const std::[w]string_view
	template<>
	struct formatter<std::string_view, wchar_t>: std::formatter<std::wstring_view, wchar_t>
	{
		template<class TFormatContext>
		auto format(std::string_view value, TFormatContext& formatContext)
		{
			auto converted = kxf::String::FromUTF8(value);
			return std::formatter<std::wstring_view, wchar_t>::format(converted.GetView(), formatContext);
		}
	};

	template<>
	struct formatter<std::wstring_view, char>: std::formatter<std::string_view, char>
	{
		template<class TFormatContext>
		auto format(std::wstring_view value, TFormatContext& formatContext)
		{
			auto utf8 = kxf::String::FromView(value).ToUTF8();
			return std::formatter<std::string_view, char>::format(utf8, formatContext);
		}
	};

	// Converting const std::[w]string
	template<>
	struct formatter<std::string, wchar_t>: formatter<std::string_view, wchar_t>
	{
		template<class TFormatContext>
		auto format(const std::string& value, TFormatContext& formatContext)
		{
			return formatter<std::string_view, wchar_t>::format(value, formatContext);
		}
	};

	template<>
	struct formatter<std::wstring, char>: formatter<std::wstring_view, char>
	{
		template<class TFormatContext>
		auto format(const std::wstring& value, TFormatContext& formatContext)
		{
			return formatter<std::wstring_view, char>::format(value, formatContext);
		}
	};

	// Enumerations
	template<class TEnum, class TChar> requires(std::is_enum_v<TEnum>)
	struct formatter<TEnum, TChar>: formatter<std::underlying_type_t<TEnum>, TChar>
	{
		template<class TFormatContext>
		auto format(TEnum value, TFormatContext& formatContext)
		{
			using Tx = std::underlying_type_t<TEnum>;
			return formatter<Tx, TChar>::format(static_cast<Tx>(value), formatContext);
		}
	};
}
