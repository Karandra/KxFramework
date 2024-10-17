#include "KxfPCH.h"
#include "DateTime.h"
#include "kxf/Localization/Locale.h"
#include <ctime>
#include <Windows.h>
#include <kxf/System/UndefWindows.h>

namespace kxf
{
	String DateTime::GetMonthName(Month month, const Locale& locale, FlagSet<UnitNameFlag> flags)
	{
		return locale.GetMonthName(month, flags).value_or(NullString);
	}
	String DateTime::GetWeekDayName(WeekDay weekDay, const Locale& locale, FlagSet<UnitNameFlag> flags)
	{
		return locale.GetWeekDayName(weekDay, flags).value_or(NullString);
	}

	wxDateTime::Tm DateTime::GetTm(const TimeZoneOffset& tz) const noexcept
	{
		return m_Value.GetTm(Private::MapTimeZone(tz));
	}
	DateTime& DateTime::SetTm(const wxDateTime::Tm& value, const TimeZoneOffset& tz) noexcept
	{
		m_Value.Set(value);
		if (!tz.IsLocal())
		{
			*this = FromTimeZone(tz);
		}
		return *this;
	}

	std::tm DateTime::GetStdTm(const TimeZoneOffset& tz) const noexcept
	{
		std::tm tm;
		tm.tm_year = GetYear(tz);
		tm.tm_mon = static_cast<int>(GetMonth(tz));
		tm.tm_hour = GetHour(tz);
		tm.tm_min = GetMinute(tz);
		tm.tm_sec = GetSecond(tz);

		tm.tm_mday = GetDay(tz);
		tm.tm_wday = static_cast<int>(Private::MapWeekDay(GetWeekDay(tz)));
		tm.tm_yday = GetDayOfYear(tz);
		tm.tm_isdst = m_Value.IsDST();

		return tm;
	}
	DateTime& DateTime::SetStdTm(const std::tm& value, const TimeZoneOffset& tz) noexcept
	{
		m_Value.Set(value);
		if (!tz.IsLocal())
		{
			*this = FromTimeZone(tz);
		}
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

	DateSpan DateTime::GetDateSpan(const TimeZoneOffset& tz) const noexcept
	{
		if (m_Value.IsValid())
		{
			int years = GetYear(tz);
			if (years == wxDateTime::Inv_Year)
			{
				years = 0;
			}

			int months = m_Value.GetMonth(Private::MapTimeZone(tz));
			if (months == wxDateTime::Inv_Month)
			{
				months = 0;
			}

			return DateSpan(years, months, 0, m_Value.GetDay(Private::MapTimeZone(tz)));
		}
		return {};
	}
	DateTime& DateTime::SetDateSpan(const DateSpan& span) noexcept
	{
		m_Value.ResetTime();
		m_Value.SetYear(span.GetYears());
		m_Value.SetMonth(static_cast<wxDateTime::Month>(span.GetMonths()));
		m_Value.SetDay(span.GetDays());

		return *this;
	}

	_SYSTEMTIME DateTime::GetSystemTime(const TimeZoneOffset& tz) const noexcept
	{
		if (m_Value.IsValid())
		{
			auto Get = [&](const DateTime& dateTime)
			{
				SYSTEMTIME systemTime = {};
				dateTime.m_Value.GetAsMSWSysTime(&systemTime);

				// Set the day of week because 'wxDatetime::GetAsMSWSysTime' doesn't set it for some reason
				systemTime.wDayOfWeek = static_cast<int>(Private::MapWeekDay(dateTime.GetWeekDay(tz)));
				return systemTime;
			};

			if (tz.IsLocal())
			{
				return Get(*this);
			}
			else
			{
				return Get(ToTimeZone(tz));
			}
		}
		return {};
	}
	DateTime& DateTime::SetSystemTime(const _SYSTEMTIME& other, const TimeZoneOffset& tz) noexcept
	{
		m_Value.SetFromMSWSysTime(other);
		if (!tz.IsLocal())
		{
			*this = FromTimeZone(tz);
		}
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
	DateTime& DateTime::SetFileTime(const _FILETIME& other, const TimeZoneOffset& tz) noexcept
	{
		SYSTEMTIME systemTime = {};
		if (::FileTimeToSystemTime(&other, &systemTime))
		{
			SetSystemTime(systemTime, tz);
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
			return m_Value.Format(wxDefaultDateTimeFormat, Private::MapTimeZone(tz));
		}
		else
		{
			return m_Value.Format(format, Private::MapTimeZone(tz));
		}
	}
	String DateTime::FormatDate(const Locale& locale, FlagSet<DateFormatFlag> flags, const TimeZoneOffset& tz) const
	{
		if (m_Value.IsValid())
		{
			FlagSet<DWORD> nativeFlags = DATE_AUTOLAYOUT;
			nativeFlags.Add(DATE_LONGDATE, flags & DateFormatFlag::Long);
			nativeFlags.Add(DATE_YEARMONTH, flags & DateFormatFlag::YearMonth);
			nativeFlags.Add(DATE_MONTHDAY, flags & DateFormatFlag::MonthDay);

			wchar_t formatted[1024] = {};
			SYSTEMTIME localTime = GetSystemTime(tz);
			if (::GetDateFormatEx(locale.m_LocaleName, *nativeFlags, &localTime, nullptr, formatted, std::size(formatted), nullptr) != 0)
			{
				return formatted;
			}
		}
		return {};
	}
	String DateTime::FormatTime(const Locale& locale, FlagSet<TimeFormatFlag> flags, const TimeZoneOffset& tz) const
	{
		if (m_Value.IsValid())
		{
			FlagSet<DWORD> nativeFlags;
			nativeFlags.Add(TIME_NOSECONDS, flags & TimeFormatFlag::NoSeconds);
			nativeFlags.Add(TIME_NOTIMEMARKER, flags & TimeFormatFlag::NoTimeMarker);
			nativeFlags.Add(TIME_NOMINUTESORSECONDS, flags & TimeFormatFlag::NoMinutes);
			nativeFlags.Add(TIME_FORCE24HOURFORMAT, flags & TimeFormatFlag::Force24Hour);

			wchar_t formatted[1024] = {};
			SYSTEMTIME localTime = GetSystemTime(tz);
			if (::GetTimeFormatEx(locale.m_LocaleName, *nativeFlags, &localTime, nullptr, formatted, std::size(formatted)) != 0)
			{
				return formatted;
			}
		}
		return {};
	}
}
