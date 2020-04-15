#pragma once
#include "Kx/Common.hpp"
#include "Kx/General/String.h"

namespace KxFramework
{
	enum class LocaleStrOption
	{
		None = -1,

		LocaleName,
		DecimalSeparator, // For example, '.' in '3.14' or ',' in '3,14'.

		ISOCountryName, // 'USA' for the United States.
		ISOLanguageName, // 'eng' for English.

		EnglishCountryName, // 'Germany' for Deutschland.
		EnglishLanguageName, // 'German' for Deutsch.
		EnglishLanguageDisplayName, // 'German (Germany)' for Deutsch (Deutschland).

		LocalizedCountryName, // 'Deutschland' for Germany.
		LocalizedLanguageName, // 'Deutsch' for German.
		LocalizedLanguageDisplayName, // 'Deutsch (Deutschland)' for German (Germany).

		NativeCountryName, // 'España' for Spain.
		NativeCurrencyName,// Native name of currency such as 'euro'.
		NativeLanguageName, // 'Հայերեն' for Armenian (Armenia).
		NativeDisplayName, // 'Deutsch (Deutschland)' for the locale German (Germany).
		NativeDigits, // Native equivalents of ASCII 0 through 9, Arabic uses '٠١٢٣٤٥ ٦٧٨٩' for example

		ShortDate, // For example, 'M/d/yyyy' indicates that September 3, 2004 is written as '9/3/2004'.
		ShortTime, // For example, if the long time format is 'h:mm:ss tt', the short time format is most likely 'h:mm tt'.
		LongDate, // For example, the Spanish (Spain) long date is "dddd, dd' de 'MMMM' de 'yyyy".
		LongTime, //  For example, a format such as "03:56'23" is valid.
	};
	enum class LocaleIntOption
	{
		None = -1,

		MeasureSystem, // This value is 0 if the metric system (Systéme International d'Units, or S.I.) is used, and 1 if the United States system is used.
		TimeFormat, // 0 if AM/PM 12-hour format, 1 if 24-hour format.
		DateFormat, // 0 - Month-Day-Year, 1 - Day-Month-Year, 2 - Year-Month-Day.
	};
}

namespace KxFramework::Localization
{
	String GetStandardLocalizedString(int id);
	String GetStandardLocalizedString(StandardID id);
}
