#include "KxStdAfx.h"
#include "Locale.h"
#include "Private/LocaleOptions.h"
#include "Kx/Utility/Common.h"

namespace
{
	using CharTraits = std::char_traits<wchar_t>;

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
		if (!std::invoke(func, MUI_LANGUAGE_NAME, &langCount, nullptr, &bufferSize))
		{
			std::wstring buffer;
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

	template<size_t N>
	bool IsEmptyLocaleString(const wchar_t(&localeName)[N]) noexcept
	{
		return *localeName == L'\0' || CharTraits::length(localeName) == 0 || !GetLocaleInfoInt(localeName, LOCALE_ITIME).has_value();
	}
}

namespace KxFramework
{
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
			return StringViewOf(buffer);
		}
		return {};
	}
	Locale Locale::GetThreadPreferred() noexcept
	{
		std::wstring buffer;
		if (CallGetLocale2(buffer, ::GetThreadPreferredUILanguages))
		{
			return StringViewOf(buffer);
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
		:Locale(String::FromUTF8(localeName))
	{
	}
	Locale::Locale(std::wstring_view localeName) noexcept
	{
		constexpr size_t bufferSize = Utility::ArraySize<decltype(m_LocaleName)>::value;
		static_assert(bufferSize >= LOCALE_NAME_MAX_LENGTH, "insufficient locale name buffer");

		CharTraits::copy(m_LocaleName, localeName.data(), std::min(std::size(m_LocaleName) - 1, localeName.length()));
	}

	bool Locale::IsNull() const noexcept
	{
		return IsEmptyLocaleString(m_LocaleName);
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
			Localization::LangID langID;
			langID.ID = PRIMARYLANGID(*value);
			langID.ID = SUBLANGID(*value);

			return langID;
		}
		return {};
	}
}
