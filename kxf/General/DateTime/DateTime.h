#pragma once
#include "Common.h"
#include "TimeSpan.h"
#include "DateSpan.h"
#include "TimeZone.h"
#include "kxf/Serialization/BinarySerializer.h"
#include "Private/Mapping.h"

struct _FILETIME;
struct _SYSTEMTIME;
namespace std
{
	struct tm;
}
namespace kxf
{
	class Locale;
}

namespace kxf
{
	class KX_API DateTime final
	{
		public:
			static constexpr int InvalidYear = wxDateTime::Inv_Year;

		public:
			static DateTime Now() noexcept
			{
				return wxDateTime::UNow();
			}
			static DateTime Today() noexcept
			{
				return wxDateTime::UNow().ResetTime();
			}

			static int GetCentury(int year) noexcept
			{
				return wxDateTime::GetCentury(year);
			}
			static int ConvertYearToBC(int year) noexcept
			{
				return wxDateTime::ConvertYearToBC(year);
			}

			static Country GetDefaultCountry() noexcept
			{
				return Private::MapCountry(wxDateTime::GetCountry());
			}
			static void SetDefaultCountry(Country country) noexcept
			{
				return wxDateTime::SetCountry(Private::MapCountry(country));
			}

			static bool IsDSTApplicable(int year = InvalidYear, Country country = Country::Default)
			{
				return wxDateTime::IsDSTApplicable(year, Private::MapCountry(country));
			}
			static DateTime GetBeginDST(int year = InvalidYear, Country country = Country::Default)
			{
				return wxDateTime::GetBeginDST(year, Private::MapCountry(country));
			}
			static DateTime GetEndDST(int year = InvalidYear, Country country = Country::Default) noexcept
			{
				return wxDateTime::GetEndDST(year, Private::MapCountry(country));
			}

			static bool IsLeapYear(int year, Calendar calendar = Calendar::Gregorian) noexcept
			{
				return wxDateTime::IsLeapYear(year, Private::MapCalendar(calendar));
			}
			static int GetNumberOfDays(int year, Calendar calendar = Calendar::Gregorian) noexcept
			{
				return wxDateTime::GetNumberOfDays(year, Private::MapCalendar(calendar));
			}
			static int GetNumberOfDays(Month month, int year = InvalidYear, Calendar calendar = Calendar::Gregorian) noexcept
			{
				return wxDateTime::GetNumberOfDays(Private::MapMonth(month), year, Private::MapCalendar(calendar));
			}

			static String GetMonthName(Month month, const Locale& locale, FlagSet<UnitNameFlag> flags = UnitNameFlag::None);
			static String GetWeekDayName(WeekDay weekDay, const Locale& locale, FlagSet<UnitNameFlag> flags = UnitNameFlag::None);

		private:
			wxDateTime m_Value;

		public:
			DateTime() noexcept
			{
			}
			DateTime(const DateTime&) noexcept = default;
			DateTime(const wxDateTime& other) noexcept
				:m_Value(other)
			{
			}

		public:
			bool IsValid() const noexcept
			{
				return m_Value.IsValid();
			}
			bool IsNegative() const noexcept
			{
				return m_Value.GetValue() < 0;
			}
			bool IsPositive() const noexcept
			{
				return m_Value.GetValue() > 0;
			}
			void MakeNull() noexcept
			{
				m_Value = {};
			}
			DateTime Clone() const noexcept
			{
				return *this;
			}

			// Accessors and modifiers
			int64_t GetValue() const noexcept
			{
				return m_Value.GetValue().GetValue();
			}
			DateTime& SetValue(int64_t value) noexcept
			{
				m_Value = wxDateTime(wxLongLong(value));
				return *this;
			}

			int GetMillisecond(const TimeZoneOffset& tz = TimeZone::Local) const noexcept
			{
				return m_Value.GetMillisecond(tz);
			}
			DateTime& SetMillisecond(int value) noexcept
			{
				m_Value.SetMillisecond(value);
				return *this;
			}

			int GetSecond(const TimeZoneOffset& tz = TimeZone::Local) const noexcept
			{
				return m_Value.GetSecond(tz);
			}
			DateTime& SetSecond(int value) noexcept
			{
				m_Value.SetSecond(value);
				return *this;
			}

			int GetMinute(const TimeZoneOffset& tz = TimeZone::Local) const noexcept
			{
				return m_Value.GetMinute(tz);
			}
			DateTime& SetMinute(int value) noexcept
			{
				m_Value.SetMinute(value);
				return *this;
			}

			int GetHour(const TimeZoneOffset& tz = TimeZone::Local) const noexcept
			{
				return m_Value.GetHour(tz);
			}
			DateTime& SetHour(int value) noexcept
			{
				m_Value.SetHour(value);
				return *this;
			}

			int GetDay(const TimeZoneOffset& tz = TimeZone::Local) const noexcept
			{
				return m_Value.GetDay(tz);
			}
			DateTime& SetDay(int value) noexcept
			{
				m_Value.SetDay(value);
				return *this;
			}

			Month GetMonth(const TimeZoneOffset& tz = TimeZone::Local) const noexcept
			{
				return Private::MapMonth(m_Value.GetMonth(tz));
			}
			DateTime& SetMonth(Month value) noexcept
			{
				m_Value.SetMonth(Private::MapMonth(value));
				return *this;
			}

			int GetYear(const TimeZoneOffset& tz = TimeZone::Local) const noexcept
			{
				return m_Value.GetYear(tz);
			}
			DateTime& SetYear(int value) noexcept
			{
				m_Value.SetYear(value);
				return *this;
			}

			int GetCentury(const TimeZoneOffset& tz = TimeZone::Local) const noexcept
			{
				return m_Value.GetCentury(tz);
			}
			int GetDayOfYear(const TimeZoneOffset& tz = TimeZone::Local) const noexcept
			{
				return m_Value.GetDayOfYear(tz);
			}
			WeekDay GetWeekDay(const TimeZoneOffset& tz = TimeZone::Local) const noexcept
			{
				return Private::MapWeekDay(m_Value.GetWeekDay(tz));
			}
			
			DateTime GetDateOnly() const noexcept
			{
				return m_Value.GetDateOnly();
			}
			DateTime& ResetTime() noexcept
			{
				m_Value.ResetTime();
				return *this;
			}

			wxDateTime::Tm GetTm(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetTm(const wxDateTime::Tm& value, const TimeZoneOffset& tz = TimeZone::Local) noexcept;

			std::tm GetStdTm(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetStdTm(const std::tm& value, const TimeZoneOffset& tz = TimeZone::Local) noexcept;

			TimeSpan GetTimeSpan(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetTimeSpan(const TimeSpan& span) noexcept;

			DateSpan GetDateSpan(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetDateSpan(const DateSpan& span) noexcept;

			time_t GetUnixTime() const noexcept
			{
				return m_Value.GetTicks();
			}
			DateTime& SetUnixTime(time_t value) noexcept
			{
				m_Value.Set(value);
				return *this;
			}

			_SYSTEMTIME GetSystemTime(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetSystemTime(const _SYSTEMTIME& other, const TimeZoneOffset& tz = TimeZone::Local) noexcept;
			
			_FILETIME GetFileTime(const TimeZoneOffset& tz = TimeZone::Local) const noexcept;
			DateTime& SetFileTime(const _FILETIME& other, const TimeZoneOffset& tz = TimeZone::Local) noexcept;

			// Comparison
			bool IsSameDate(const DateTime& other) const noexcept
			{
				return m_Value.IsSameDate(other.m_Value);
			}
			bool IsSameTime(const DateTime& other) const noexcept
			{
				return m_Value.IsSameTime(other.m_Value);
			}
			bool IsEqualUpTo(const DateTime& other, const TimeSpan& span) const noexcept
			{
				return m_Value.IsEqualUpTo(other.m_Value, span);
			}

			bool IsEarlierThan(const DateTime& other) const noexcept
			{
				return m_Value.IsEarlierThan(other.m_Value);
			}
			bool IsLaterThan(const DateTime& other) const noexcept
			{
				return m_Value.IsLaterThan(other.m_Value);
			}

			bool IsBetween(const DateTime& left, const DateTime& right) const noexcept
			{
				return m_Value.IsStrictlyBetween(left.m_Value, right.m_Value);
			}
			bool IsBetweenInclusive(const DateTime& left, const DateTime& right) const noexcept
			{
				return m_Value.IsBetween(left.m_Value, right.m_Value);
			}

			auto operator<=>(const DateTime& other) const noexcept
			{
				return GetValue() <=> other.GetValue();
			}
			bool operator==(const DateTime& other) const noexcept
			{
				return GetValue() == other.GetValue();
			}

			// Arithmetics
			DateSpan Difference(const DateTime& other) const noexcept
			{
				return m_Value.DiffAsDateSpan(other.m_Value);
			}

			DateTime operator+(const TimeSpan& other) const noexcept
			{
				return m_Value + other;
			}
			DateTime operator-(const TimeSpan& other) const noexcept
			{
				return m_Value - other;
			}
			DateTime operator+(const DateSpan& other) const noexcept
			{
				return m_Value + other;
			}
			DateTime operator-(const DateSpan& other) const noexcept
			{
				return m_Value - other;
			}
			TimeSpan operator-(const DateTime& other) const noexcept
			{
				return m_Value - other.m_Value;
			}

			DateTime& operator+=(const TimeSpan& other) noexcept
			{
				m_Value += other;
				return *this;
			}
			DateTime& operator-=(const TimeSpan& other) noexcept
			{
				m_Value -= other;
				return *this;
			}
			DateTime& operator+=(const DateSpan& other) noexcept
			{
				m_Value += other;
				return *this;
			}
			DateTime& operator-=(const DateSpan& other) noexcept
			{
				m_Value -= other;
				return *this;
			}

			// Formatting and parsing
			String Format(const String& format = {}, const TimeZoneOffset& tz = TimeZone::Local) const;
			String Format(const char* format, const TimeZoneOffset& tz = TimeZone::Local) const
			{
				return Format(String(format), tz);
			}
			String Format(const wchar_t* format, const TimeZoneOffset& tz = TimeZone::Local) const
			{
				return Format(String(format), tz);
			}

			String FormatDate(const Locale& locale, FlagSet<DateFormatFlag> flags = DateFormatFlag::None, const TimeZoneOffset& tz = TimeZone::Local) const;
			String FormatTime(const Locale& locale, FlagSet<TimeFormatFlag> flags = TimeFormatFlag::None, const TimeZoneOffset& tz = TimeZone::Local) const;

			String FormatISODate() const
			{
				return m_Value.FormatISODate();
			}
			String FormatISOTime() const
			{
				return m_Value.FormatISOTime();
			}
			String FormatISOCombined(const UniChar& sep = 'T') const
			{
				return m_Value.FormatISOCombined(sep.ToASCII().value_or('T'));
			}

			bool ParseDate(const String& date)
			{
				wxString::const_iterator it;
				return m_Value.ParseDate(date, &it);
			}
			bool ParseTime(const String& time)
			{
				wxString::const_iterator it;
				return m_Value.ParseTime(time, &it);
			}
			bool ParseCombined(const String& dateTime)
			{
				wxString::const_iterator it;
				return m_Value.ParseDateTime(dateTime, &it);
			}

			bool ParseFormat(const String& dateTime)
			{
				wxString::const_iterator it;
				return m_Value.ParseFormat(dateTime, &it);
			}
			bool ParseFormat(const String& dateTime, const String& format)
			{
				wxString::const_iterator it;
				return m_Value.ParseFormat(dateTime, format, &it);
			}
			bool ParseFormat(const String& dateTime, const String& format, const DateTime& dateDefault)
			{
				wxString::const_iterator it;
				return m_Value.ParseFormat(dateTime, format, dateDefault, &it);
			}

			bool ParseISODate(const String& date)
			{
				return m_Value.ParseISODate(date);
			}
			bool ParseISOTime(const String& time)
			{
				return m_Value.ParseISOTime(time);
			}
			bool ParseISOCombined(const String& dateTime, const UniChar& sep = 'T')
			{
				return m_Value.ParseISOCombined(dateTime, sep.ToASCII().value_or('T'));
			}

			bool ParseRFC822Date(const String& dateTime, wxString::const_iterator* end)
			{
				return m_Value.ParseRfc822Date(dateTime, end);
			}

			// Calendar calculations
			bool IsWorkDay(Country country) const noexcept
			{
				return m_Value.IsWorkDay(Private::MapCountry(country));
			}

			bool SetToWeekDay(WeekDay weekday, int n = 1, Month month = Month::None, int year = InvalidYear) noexcept
			{
				return m_Value.SetToWeekDay(Private::MapWeekDay(weekday), n, Private::MapMonth(month), year);
			}
			bool SetToLastWeekDay(WeekDay weekday, Month month = Month::None, int year = InvalidYear) noexcept
			{
				return m_Value.SetToLastWeekDay(Private::MapWeekDay(weekday), Private::MapMonth(month), year);
			}
			DateTime& SetToWeekDayInSameWeek(WeekDay weekday, WeekDayOption options = WeekDayOption::MondayFirst) noexcept
			{
				m_Value.SetToWeekDayInSameWeek(Private::MapWeekDay(weekday), Private::MapWeekDayFlag(options));
				return *this;
			}
			DateTime& SetToNextWeekDay(WeekDay weekday) noexcept
			{
				m_Value.SetToNextWeekDay(Private::MapWeekDay(weekday));
				return *this;
			}
			DateTime& SetToPrevWeekDay(WeekDay weekday) noexcept
			{
				m_Value.SetToPrevWeekDay(Private::MapWeekDay(weekday));
				return *this;
			}

			DateTime& SetToLastMonthDay(Month month = Month::None, int year = InvalidYear) noexcept
			{
				m_Value.SetToLastMonthDay(Private::MapMonth(month), year);
				return *this;
			}
			DateTime& SetToYearDay(int day) noexcept
			{
				m_Value.SetToYearDay(day);
				return *this;
			}

			// Astronomical/Historical functions
			double GetRataDie() const noexcept
			{
				return m_Value.GetRataDie();
			}

			double GetJulianDayNumber() const noexcept
			{
				return m_Value.GetJulianDayNumber();
			}
			double GetModifiedJulianDayNumber() const noexcept
			{
				return m_Value.GetModifiedJulianDayNumber();
			}
			DateTime& SetJulianDayNumber(double jdn) noexcept
			{
				m_Value.Set(jdn);
				return *this;
			}

			// Time Zone and DST support
			DateTime ToTimeZone(const TimeZoneOffset& tz, bool noDST = false) const noexcept
			{
				return m_Value.ToTimezone(tz, noDST);
			}
			DateTime FromTimeZone(const TimeZoneOffset& tz, bool noDST = false) const noexcept
			{
				return m_Value.FromTimezone(tz, noDST);
			}
			DateTime ToUTC(bool noDST = false) const noexcept
			{
				return m_Value.ToUTC(noDST);
			}
			bool IsDST(Country country = Country::Default) const noexcept
			{
				return m_Value.IsDST(Private::MapCountry(country));
			}

		public:
			explicit operator bool() const noexcept
			{
				return IsValid();
			}
			bool operator!() const noexcept
			{
				return !IsValid();
			}

			DateTime& operator=(const DateTime&) noexcept = default;
			DateTime& operator=(const wxDateTime& other) noexcept
			{
				m_Value = other;
				return *this;
			}
			operator wxDateTime() const noexcept
			{
				return m_Value;
			}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<DateTime> final
	{
		uint64_t Serialize(IOutputStream& stream, const DateTime& value) const
		{
			return Serialization::WriteObject(stream, value.GetValue());
		}
		uint64_t Deserialize(IInputStream& stream, DateTime& value) const
		{
			int64_t buffer = 0;
			auto read = Serialization::ReadObject(stream, buffer);
			value.SetValue(buffer);

			return read;
		}
	};
}

namespace std
{
	template<>
	struct hash<kxf::DateTime> final
	{
		size_t operator()(const kxf::DateTime& dateTime) const noexcept
		{
			return std::hash<int64_t>()(dateTime.GetValue());
		}
	};
}
