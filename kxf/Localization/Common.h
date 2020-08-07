#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/String.h"
#include "kxf/General/ResourceID.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/FileSystem/FileItem.h"
#include "kxf/FileSystem/IFileSystem.h"

namespace kxf
{
	class Locale;
	class DynamicLibrary;

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

		NativeMonthJanuary,
		NativeMonthFebruary,
		NativeMonthMarch,
		NativeMonthApril,
		NativeMonthMay,
		NativeMonthJune,
		NativeMonthJuly,
		NativeMonthAugust,
		NativeMonthSeptember,
		NativeMonthOctober,
		NativeMonthNovember,
		NativeMonthDecember,

		NativeMonthAbbrJanuary,
		NativeMonthAbbrFebruary,
		NativeMonthAbbrMarch,
		NativeMonthAbbrApril,
		NativeMonthAbbrMay,
		NativeMonthAbbrJune,
		NativeMonthAbbrJuly,
		NativeMonthAbbrAugust,
		NativeMonthAbbrSeptember,
		NativeMonthAbbrOctober,
		NativeMonthAbbrNovember,
		NativeMonthAbbrDecember,

		NativeDayMonday,
		NativeDayTuesday,
		NativeDayWednesday,
		NativeDayThursday,
		NativeDayFriday,
		NativeDaySaturday,
		NativeDaySunday,

		NativeDayAbbrMonday,
		NativeDayAbbrTuesday,
		NativeDayAbbrWednesday,
		NativeDayAbbrThursday,
		NativeDayAbbrFriday,
		NativeDayAbbrSaturday,
		NativeDayAbbrSunday
	};
	enum class LocaleIntOption
	{
		None = -1,

		MeasureSystem, // This value is 0 if the metric system (Systéme International d'Units, or S.I.) is used, and 1 if the United States system is used.
		TimeFormat, // 0 if AM/PM 12-hour format, 1 if 24-hour format.
		DateFormat, // 0 - Month-Day-Year, 1 - Day-Month-Year, 2 - Year-Month-Day.
	};
}

namespace kxf::Localization
{
	String GetStandardString(int id);
	String GetStandardString(StdID id);

	size_t SearchPackages(const IFileSystem& fileSystem, const FSPath& directory, std::function<bool(Locale, FileItem)> func);
	size_t SearchPackages(const DynamicLibrary& library, std::function<bool(Locale, FileItem)> func);
}
