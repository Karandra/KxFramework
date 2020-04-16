#include "stdafx.h"
#include "LocaleOptions.h"
#include <WinNls.h>

namespace KxFramework::Localization::Private
{
	std::optional<uint32_t> MapLocaleOption(LocaleStrOption option) noexcept
	{
		switch (option)
		{
			case LocaleStrOption::LocaleName:
			{
				return LOCALE_SNAME;
			}
			case LocaleStrOption::DecimalSeparator:
			{
				return LOCALE_SDECIMAL;
			}

			case LocaleStrOption::ISOCountryName:
			{
				return LOCALE_SISO3166CTRYNAME2;
			}
			case LocaleStrOption::ISOLanguageName:
			{
				return LOCALE_SISO639LANGNAME2;
			}

			case LocaleStrOption::EnglishCountryName:
			{
				return LOCALE_SENGLISHCOUNTRYNAME;
			}
			case LocaleStrOption::EnglishLanguageName:
			{
				return LOCALE_SENGLISHLANGUAGENAME;
			}
			case LocaleStrOption::EnglishLanguageDisplayName:
			{
				return LOCALE_SENGLISHDISPLAYNAME;
			}

			case LocaleStrOption::LocalizedCountryName:
			{
				return LOCALE_SLOCALIZEDCOUNTRYNAME;
			}
			case LocaleStrOption::LocalizedLanguageName:
			{
				return LOCALE_SLOCALIZEDLANGUAGENAME;
			}
			case LocaleStrOption::LocalizedLanguageDisplayName:
			{
				return LOCALE_SLOCALIZEDDISPLAYNAME;
			}

			case LocaleStrOption::NativeCountryName:
			{
				return LOCALE_SNATIVECOUNTRYNAME;
			}
			case LocaleStrOption::NativeCurrencyName:
			{
				return LOCALE_SNATIVECURRNAME;
			}
			case LocaleStrOption::NativeLanguageName:
			{
				return LOCALE_SNATIVELANGUAGENAME;
			}
			case LocaleStrOption::NativeDisplayName:
			{
				return LOCALE_SNATIVEDISPLAYNAME;
			}
			case LocaleStrOption::NativeDigits:
			{
				return LOCALE_SNATIVEDIGITS;
			}

			case LocaleStrOption::ShortDate:
			{
				return LOCALE_SSHORTDATE;
			}
			case LocaleStrOption::ShortTime:
			{
				return LOCALE_SSHORTTIME;
			}
			case LocaleStrOption::LongDate:
			{
				return LOCALE_SLONGDATE;
			}
			case LocaleStrOption::LongTime:
			{
				return LOCALE_STIMEFORMAT;
			}
		};
		return {};
	}
	std::optional<uint32_t> MapLocaleOption(LocaleIntOption option) noexcept
	{
		switch (option)
		{
			case LocaleIntOption::MeasureSystem:
			{
				return LOCALE_IMEASURE;
			}
			case LocaleIntOption::TimeFormat:
			{
				return LOCALE_ITIME;
			}
			case LocaleIntOption::DateFormat:
			{
				return LOCALE_IDATE;
			}
		};
		return {};
	}
}
