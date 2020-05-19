#include "stdafx.h"
#include "DateTime.h"
#include "Kx/Localization/Locale.h"
#include <ctime>
#include <Windows.h>
#include <Kx/System/UndefWindows.h>

namespace KxFramework
{
	String DateTime::GetMonthName(Month month, const Locale& locale, UnitNameFlag flags)
	{
		return locale.GetMonthName(month, flags).value_or(NullString);
	}
	String DateTime::GetWeekDayName(WeekDay weekDay, const Locale& locale, UnitNameFlag flags)
	{
		return locale.GetWeekDayName(weekDay, flags).value_or(NullString);
	}

	std::tm DateTime::GetStdTm(const TimeZoneOffset& tz) const noexcept
	{
		std::tm tm;
		tm.tm_year = GetYear(tz);
		tm.tm_mon = ToInt(GetMonth(tz));
		tm.tm_hour = GetHour(tz);
		tm.tm_min = GetMinute(tz);
		tm.tm_sec = GetSecond(tz);

		tm.tm_mday = GetDayOfMonth(tz);
		tm.tm_wday = static_cast<int>(Private::MapWeekDay(GetWeekDay(tz)));
		tm.tm_yday = GetDayOfYear(tz);
		tm.tm_isdst = m_Value.IsDST();

		return tm;
	}
	DateTime& DateTime::SetTm(const std::tm& value) noexcept
	{
		m_Value.Set(value);
		return *this;
	}

	TimeSpan DateTime::GetTimeSpan(const TimeZoneOffset& tz) const noexcept
	{
		return TimeSpan::Seconds(GetUnixTime());
	}
	DateTime& DateTime::SetTimeSpan(const TimeSpan& span) noexcept
	{
		SetUnixTime(span.GetSeconds());
		return *this;
	}

	_SYSTEMTIME DateTime::GetSystemTime(const TimeZoneOffset& tz) const noexcept
	{
		SYSTEMTIME systemTime = {};
		m_Value.GetAsMSWSysTime(&systemTime);
		return systemTime;
	}
	DateTime& DateTime::SetSystemTime(const _SYSTEMTIME& other) noexcept
	{
		m_Value.SetFromMSWSysTime(other);
		return *this;
	}

	_FILETIME DateTime::GetFileTime(const TimeZoneOffset& tz) const noexcept
	{
		SYSTEMTIME systemTime = GetSystemTime(tz);
		if (FILETIME fileTIme = {}; ::SystemTimeToFileTime(&systemTime, &fileTIme))
		{
			return fileTIme;
		}
		return {};
	}
	DateTime& DateTime::SetFileTime(const _FILETIME& other) noexcept
	{
		SYSTEMTIME systemTime = {};
		if (::FileTimeToSystemTime(&other, &systemTime))
		{
			m_Value.SetFromMSWSysTime(systemTime);
		}
		else
		{
			m_Value = wxInvalidDateTime;
		}
		return *this;
	}

	String DateTime::Format(const String& format, const TimeZoneOffset& tz) const
	{
		if (format.IsEmpty())
		{
			return m_Value.Format(wxDefaultDateTimeFormat, tz);
		}
		else
		{
			return m_Value.Format(format, tz);
		}
	}
	String DateTime::FormatDate(const Locale& locale, DateFormatFlag flags, const TimeZoneOffset& tz) const
	{
		if (m_Value.IsValid())
		{
			DWORD nativeFlags = DATE_AUTOLAYOUT;
			Utility::AddFlagRef(nativeFlags, DATE_LONGDATE, flags & DateFormatFlag::Long);
			Utility::AddFlagRef(nativeFlags, DATE_YEARMONTH, flags & DateFormatFlag::YearMonth);
			Utility::AddFlagRef(nativeFlags, DATE_MONTHDAY, flags & DateFormatFlag::MonthDay);

			wchar_t formatted[1024] = {};
			SYSTEMTIME localTime = GetSystemTime(tz);
			if (::GetDateFormatEx(locale.m_LocaleName, nativeFlags, &localTime, nullptr, formatted, std::size(formatted), nullptr) != 0)
			{
				return formatted;
			}
		}
		return {};
	}
	String DateTime::FormatTime(const Locale& locale, TimeFormatFlag flags, const TimeZoneOffset& tz) const
	{
		if (m_Value.IsValid())
		{
			DWORD nativeFlags = 0;
			Utility::AddFlagRef(nativeFlags, TIME_NOSECONDS, flags & TimeFormatFlag::NoSeconds);
			Utility::AddFlagRef(nativeFlags, TIME_NOTIMEMARKER, flags & TimeFormatFlag::NoTimeMarker);
			Utility::AddFlagRef(nativeFlags, TIME_NOMINUTESORSECONDS, flags & TimeFormatFlag::NoMinutes);
			Utility::AddFlagRef(nativeFlags, TIME_FORCE24HOURFORMAT, flags & TimeFormatFlag::Force24Hour);

			wchar_t formatted[1024] = {};
			SYSTEMTIME localTime = GetSystemTime(tz);
			if (::GetTimeFormatEx(locale.m_LocaleName, nativeFlags, &localTime, nullptr, formatted, std::size(formatted)) != 0)
			{
				return formatted;
			}
		}
		return {};
	}
}
