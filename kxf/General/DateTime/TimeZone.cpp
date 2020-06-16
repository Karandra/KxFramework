#include "stdafx.h"
#include "TimeZone.h"
#include <chrono>

namespace
{
	constexpr int g_LocalOffset = -1;
}

namespace kxf
{
	wxDateTime::TimeZone TimeZoneOffset::ToWxOffset() const noexcept
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
	void TimeZoneOffset::FromWxOffset(const wxDateTime::TimeZone& other) noexcept
	{
		if (other.IsLocal())
		{
			m_Offset = TimeSpan::Milliseconds(g_LocalOffset);
		}
		else
		{
			m_Offset = TimeSpan::Seconds(other.GetOffset());
		}
	}
	void TimeZoneOffset::FromTimeZone(TimeZone tz) noexcept
	{
		FromWxOffset(wxDateTime::TimeZone(tz));
	}

	bool TimeZoneOffset::IsLocal() const noexcept
	{
		return m_Offset.GetMilliseconds() == g_LocalOffset;
	}
	TimeSpan TimeZoneOffset::GetOffset() const noexcept
	{
		return TimeSpan::Seconds(ToWxOffset().GetOffset());
	}
}
