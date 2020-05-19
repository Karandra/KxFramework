#pragma once
#include "../Common.h"
#include "../String.h"
#include <wx/datetime.h>

namespace KxFramework
{
	enum class DateFormatFlag
	{
		None = 0,

		Long = 1 << 0,
		YearMonth = 1 << 1,
		MonthDay = 1 << 2,
	};
	enum class TimeFormatFlag
	{
		None = 0,

		NoMinutes = 1 << 0,
		NoSeconds = 1 << 1,
		NoTimeMarker = 1 << 2,
		Force24Hour = 1 << 3
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowEverything(DateFormatFlag);
		Kx_EnumClass_AllowEverything(TimeFormatFlag);
	}
}
