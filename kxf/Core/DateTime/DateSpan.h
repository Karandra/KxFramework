#pragma once
#include "Common.h"
#include "TimeSpan.h"

namespace kxf
{
	class Locale;
	class ITimeClock;
}

namespace kxf
{
	class KX_API DateSpan final
	{
		public:
			static DateSpan Days(int days) noexcept
			{
				return wxDateSpan::Days(days);
			}
			static DateSpan Weeks(int days) noexcept
			{
				return wxDateSpan::Weeks(days);
			}
			static DateSpan Months(int months) noexcept
			{
				return wxDateSpan::Months(months);
			}
			static DateSpan Years(int years) noexcept
			{
				return wxDateSpan::Years(years);
			}

		private:
			wxDateSpan m_Value;

		public:
			DateSpan() noexcept
			{
			}
			DateSpan(const DateSpan&) noexcept = default;
			DateSpan(const TimeSpan& other) noexcept
				:m_Value(wxDateSpan::Days(other.GetDays()))
			{
			}
			DateSpan(const wxDateSpan& other) noexcept
				:m_Value(other)
			{
			}
			DateSpan(int years, int months, int weeks, int days)
				:m_Value(years, months, weeks, days)
			{
			}

		public:
			bool IsNull() const noexcept
			{
				return m_Value.GetDays() == 0 && m_Value.GetWeeks() == 0 && m_Value.GetMonths() == 0 && m_Value.GetYears() == 0;
			}
			bool IsNegative() const noexcept
			{
				return m_Value.GetDays() < 0 && m_Value.GetWeeks() < 0 && m_Value.GetMonths() < 0 && m_Value.GetYears() < 0;
			}
			bool IsPositive() const noexcept
			{
				return m_Value.GetDays() > 0 && m_Value.GetWeeks() > 0 && m_Value.GetMonths() > 0 && m_Value.GetYears() > 0;
			}

			int GetDays() const noexcept
			{
				return m_Value.GetDays();
			}
			int GetWeeks() const noexcept
			{
				return m_Value.GetWeeks();
			}
			int GetMonths() const noexcept
			{
				return m_Value.GetMonths();
			}
			int GetYears() const noexcept
			{
				return m_Value.GetYears();
			}

			int GetTotalDays() const noexcept
			{
				return m_Value.GetTotalDays();
			}
			int GetTotalMonths() const noexcept
			{
				return m_Value.GetTotalMonths();
			}

			DateSpan Abs() const noexcept
			{
				return wxDateSpan(std::abs(m_Value.GetYears()), std::abs(m_Value.GetMonths()), std::abs(m_Value.GetWeeks()), std::abs(m_Value.GetDays()));
			}
			DateSpan Negate() const noexcept
			{
				return m_Value.Negate();
			}

			// Compare two time spans, works with the absolute values
			bool IsLongerThan(const TimeSpan& other) const noexcept
			{
				return Abs() > other.Abs();
			}
			bool IsShorterThan(const TimeSpan& other) const noexcept
			{
				return Abs() < other.Abs();
			}

			String Format(const String& format) const;
			String Format(const char* format) const
			{
				return Format(String(format));
			}
			String Format(const wchar_t* format) const
			{
				return Format(String(format));
			}
			String Format(const Locale& locale, DateFormatFlag flags = DateFormatFlag::None) const;

		public:
			DateSpan& operator=(const DateSpan&) noexcept = default;
			DateSpan& operator=(const TimeSpan& other) noexcept
			{
				m_Value = wxDateSpan::Days(other.GetDays());
				return *this;
			}
			DateSpan& operator=(const wxDateSpan& other) noexcept
			{
				m_Value = other;
				return *this;
			}
			operator wxDateSpan() const noexcept
			{
				return m_Value;
			}

			std::strong_ordering operator<=>(const DateSpan& other) const noexcept
			{
				if (auto cmp = m_Value.GetYears() <=> other.GetYears(); cmp != 0)
				{
					return cmp;
				}
				if (auto cmp = m_Value.GetMonths() <=> other.GetMonths(); cmp != 0)
				{
					return cmp;
				}
				if (auto cmp = m_Value.GetWeeks() <=> other.GetWeeks(); cmp != 0)
				{
					return cmp;
				}
				return m_Value.GetDays() <=> other.GetDays();
			}
			bool operator==(const DateSpan& other) const noexcept
			{
				return m_Value == other.m_Value;
			}

			DateSpan& operator+=(const DateSpan& other) noexcept
			{
				m_Value += other.m_Value;
				return *this;
			}
			DateSpan& operator-=(const DateSpan& other) noexcept
			{
				m_Value -= other.m_Value;
				return *this;
			}
			DateSpan& operator*=(int multiplier) noexcept
			{
				m_Value *= multiplier;
				return *this;
			}

			DateSpan operator-() const noexcept
			{
				return Negate();
			}
			DateSpan operator+(const DateSpan& other) const noexcept
			{
				return m_Value + other.m_Value;
			}
			DateSpan operator-(const DateSpan& other) const noexcept
			{
				return m_Value - other.m_Value;
			}
			DateSpan operator*(int multiplier) const noexcept
			{
				return m_Value * multiplier;
			}
	};
}
