#include "stdafx.h"
#include "Locale.h"
#include "Private/LocaleOptions.h"
#include "Kx/Utility/Common.h"

namespace
{
	using CharTraits = std::char_traits<wchar_t>;
	constexpr wchar_t g_InvalidChar = std::numeric_limits<wchar_t>::max();

	template<size_t N>
	void CopyToBuffer(wchar_t(&buffer)[N], std::wstring_view localeName) noexcept
	{
		CharTraits::copy(buffer, localeName.data(), std::min(std::size(buffer) - 1, localeName.length()));
	}

	template<size_t N>
	void InvalidateBuffer(wchar_t(&buffer)[N]) noexcept
	{
		buffer[0] = std::numeric_limits<wchar_t>::max();
		buffer[1] = L'\0';
	}

	template<size_t N, class TFunc>
	bool CallGetLocale1(wchar_t(&localeName)[N], TFunc&& func) noexcept
	{
		return std::invoke(func, localeName, std::size(localeName));
	}

	template<class TFunc>
	bool CallGetLocale2(std::wstring& buffer, TFunc&& func)
	{
		ULONG langCount = 0;
		ULONG bufferSize = 0;
		if (std::invoke(func, MUI_LANGUAGE_NAME, &langCount, nullptr, &bufferSize))
		{
			buffer.resize(bufferSize);
			return std::invoke(func, MUI_LANGUAGE_NAME, &langCount, buffer.data(), &bufferSize);
		}
		return false;
	}

	template<class T>
	std::optional<uint32_t> GetLocaleInfoInt(T&& localeName, uint32_t option) noexcept
	{
		DWORD result = std::numeric_limits<DWORD>::max();
		if (::GetLocaleInfoEx(localeName, option|LOCALE_RETURN_NUMBER, reinterpret_cast<wchar_t*>(&result), sizeof(result)) != 0)
		{
			return result;
		}
		return {};
	}
	
	template<class T>
	std::optional<KxFramework::String> GetLocaleInfoString(T&& localeName, uint32_t option) noexcept
	{
		using namespace KxFramework;

		int length = ::GetLocaleInfoEx(localeName, option, nullptr, 0);
		if (length != 0)
		{
			String value;
			if (::GetLocaleInfoEx(localeName, option, wxStringBuffer(value, length), length) != 0)
			{
				return value;
			}
		}
		return {};
	}
}

namespace KxFramework
{
	Locale Locale::GetInvariant() noexcept
	{
		Locale locale;
		CharTraits::copy(locale.m_LocaleName, LOCALE_NAME_INVARIANT, std::size(LOCALE_NAME_INVARIANT));

		return locale;
	}
	Locale Locale::GetUserDefault() noexcept
	{
		Locale locale;
		if (CallGetLocale1(locale.m_LocaleName, ::GetUserDefaultLocaleName))
		{
			return locale;
		}
		return {};
	}
	Locale Locale::GetSystemDefault() noexcept
	{
		Locale locale;
		if (CallGetLocale1(locale.m_LocaleName, ::GetSystemDefaultLocaleName))
		{
			return locale;
		}
		return {};
	}
	Locale Locale::GetSystemPreferred() noexcept
	{
		std::wstring buffer;
		if (CallGetLocale2(buffer, ::GetSystemPreferredUILanguages))
		{
			return StringViewOf(buffer.c_str());
		}
		return {};
	}
	Locale Locale::GetThreadPreferred() noexcept
	{
		std::wstring buffer;
		if (CallGetLocale2(buffer, ::GetThreadPreferredUILanguages))
		{
			return StringViewOf(buffer.c_str());
		}
		return {};
	}

	Locale Locale::FromLCID(uint32_t lcid) noexcept
	{
		Locale locale;
		if (::LCIDToLocaleName(lcid, locale.m_LocaleName, std::size(locale.m_LocaleName), LOCALE_ALLOW_NEUTRAL_NAMES) != 0)
		{
			return locale;
		}
		return {};
	}
	Locale Locale::FromLangID(Localization::LangID langID) noexcept
	{
		return FromLCID(MAKELCID(langID.ID, langID.SortOrder));
	}

	Locale::Locale(std::string_view localeName) noexcept
	{
		if (!localeName.empty())
		{
			String temp = String::FromUTF8(localeName);
			CopyToBuffer(m_LocaleName, StringViewOf(temp));
		}
		else
		{
			InvalidateBuffer(m_LocaleName);
		}
	}
	Locale::Locale(std::wstring_view localeName) noexcept
	{
		static_assert(Utility::ArraySize<decltype(m_LocaleName)>::value >= LOCALE_NAME_MAX_LENGTH, "insufficient locale name buffer");

		if (!localeName.empty())
		{
			CopyToBuffer(m_LocaleName, localeName);
		}
		else
		{
			InvalidateBuffer(m_LocaleName);
		}
	}

	bool Locale::IsNull() const noexcept
	{
		return *m_LocaleName == g_InvalidChar || !::IsValidLocaleName(m_LocaleName);
	}
	bool Locale::IsInvariant() const noexcept
	{
		return !IsNull() && std::wstring_view(m_LocaleName) == LOCALE_NAME_INVARIANT;
	}

	std::optional<String> Locale::GetOption(LocaleStrOption option) const
	{
		if (auto nativeOption = Localization::Private::MapLocaleOption(option))
		{
			return GetLocaleInfoString(m_LocaleName, *nativeOption);
		}
		return {};
	}
	std::optional<uint32_t> Locale::GetOption(LocaleIntOption option) const noexcept
	{
		if (auto nativeOption = Localization::Private::MapLocaleOption(option))
		{
			return GetLocaleInfoInt(m_LocaleName, *nativeOption);
		}
		return {};
	}

	bool Locale::SetOption(LocaleStrOption option, const String& value)
	{
		return false;
	}
	bool Locale::SetOption(LocaleIntOption option, uint32_t value)
	{
		return false;
	}

	std::optional<uint32_t> Locale::GetLCID() const noexcept
	{
		uint32_t lcid = ::LocaleNameToLCID(m_LocaleName, 0);
		if (lcid != 0)
		{
			return lcid;
		}
		return {};
	}
	std::optional<Localization::LangID> Locale::GetLangID() const noexcept
	{
		if (auto value = GetLocaleInfoInt(m_LocaleName, LOCALE_ILANGUAGE))
		{
			return Localization::LangID(PRIMARYLANGID(*value), SUBLANGID(*value));
		}
		return {};
	}

	String Locale::FormatDate(const wxDateTime& dateTime, DateFormatFlag flags) const
	{
		if (dateTime.IsValid())
		{
			DWORD nativeFlags = DATE_AUTOLAYOUT;
			Utility::AddFlagRef(nativeFlags, DATE_LONGDATE, flags & DateFormatFlag::Long);
			Utility::AddFlagRef(nativeFlags, DATE_YEARMONTH, flags & DateFormatFlag::YearMonth);
			Utility::AddFlagRef(nativeFlags, DATE_MONTHDAY, flags & DateFormatFlag::MonthDay);

			SYSTEMTIME localTime = {};
			dateTime.GetAsMSWSysTime(&localTime);

			wchar_t formatted[1024] = {};
			if (::GetDateFormatEx(m_LocaleName, nativeFlags, &localTime, nullptr, formatted, std::size(formatted), nullptr) != 0)
			{
				return formatted;
			}
		}
		return {};
	}
	String Locale::FormatTime(const wxDateTime& dateTime, TimeFormatFlag flags) const
	{
		if (dateTime.IsValid())
		{
			DWORD nativeFlags = 0;
			Utility::AddFlagRef(nativeFlags, TIME_NOSECONDS, flags & TimeFormatFlag::NoSeconds);
			Utility::AddFlagRef(nativeFlags, TIME_NOTIMEMARKER, flags & TimeFormatFlag::NoTimeMarker);
			Utility::AddFlagRef(nativeFlags, TIME_NOMINUTESORSECONDS, flags & TimeFormatFlag::NoMinutes);
			Utility::AddFlagRef(nativeFlags, TIME_FORCE24HOURFORMAT, flags & TimeFormatFlag::Force24Hour);

			SYSTEMTIME localTime = {};
			dateTime.GetAsMSWSysTime(&localTime);

			wchar_t formatted[1024] = {};
			if (::GetTimeFormatEx(m_LocaleName, nativeFlags, &localTime, nullptr, formatted, std::size(formatted)) != 0)
			{
				return formatted;
			}
		}
		return {};
	}
}
