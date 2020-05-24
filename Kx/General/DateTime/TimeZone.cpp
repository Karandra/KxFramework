#include "stdafx.h"
#include "TimeZone.h"
#include <chrono>

namespace
{
	constexpr int g_LocalOffset = -1;
}

namespace kxf
{
	bool TimeZoneOffset::IsLocal() const noexcept
	{
		return m_Offset.GetMilliseconds() == g_LocalOffset;
	}
	TimeSpan TimeZoneOffset::GetOffset() const noexcept
	{
		return TimeSpan::Seconds(static_cast<wxDateTime::TimeZone>(*this).GetOffset());
	}

	TimeZoneOffset& TimeZoneOffset::operator=(TimeZone tz) noexcept
	{
		*this = wxDateTime::TimeZone(tz);
		return *this;
	}
	TimeZoneOffset& TimeZoneOffset::operator=(const wxDateTime::TimeZone& other) noexcept
	{
		if (other.IsLocal())
		{
			m_Offset = TimeSpan::Milliseconds(g_LocalOffset);
		}
		else
		{
			m_Offset = TimeSpan::Milliseconds(other.GetOffset());
		}
		return *this;
	}
	TimeZoneOffset::operator wxDateTime::TimeZone() const noexcept
	{
		if (IsLocal())
		{
			return g_LocalOffset;
		}
		else
		{
			return m_Offset.GetSeconds();
		}
	}
}
