#pragma once
#include "../Common.h"
#include "../String.h"
#include <wx/datetime.h>

namespace kxf
{
	enum class DateFormatFlag: uint32_t
	{
		None = 0,

		Long = 1 << 0,
		YearMonth = 1 << 1,
		MonthDay = 1 << 2,
	};
	Kx_DeclareFlagSet(DateFormatFlag);

	enum class TimeFormatFlag: uint32_t
	{
		None = 0,

		NoMinutes = 1 << 0,
		NoSeconds = 1 << 1,
		NoTimeMarker = 1 << 2,
		Force24Hour = 1 << 3
	};
	Kx_DeclareFlagSet(TimeFormatFlag);

	enum class Month: uint32_t
	{
		None = -1,

		January,
		February,
		March,
		April,
		May,
		June,
		July,
		August,
		September,
		October,
		November,
		December
	};
	enum class WeekDay: uint32_t
	{
		None = -1,

		Monday,
		Tuesday,
		Wednesday,
		Thursday,
		Friday,
		Saturday,
		Sunday
	};
	enum class WeekDayOption: uint32_t
	{
		None = 0,

		DefaultFirst,
		MondayFirst,
		SundayFirst
	};
	enum class Country: uint32_t
	{
		None = -1,
		Default = 0,

		France,
		Germany,
		Russia,
		USA,
		UK
	};
	enum class Calendar: uint32_t
	{
		Gregorian,
		Julian
	};
}
