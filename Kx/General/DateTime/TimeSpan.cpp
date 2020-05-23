#include "stdafx.h"
#include "TimeSpan.h"
#include "TimeClock.h"

namespace KxFramework
{
	TimeSpan TimeSpan::Now() noexcept
	{
		return SteadyClock().Now();
	}
	TimeSpan TimeSpan::Now(const ITimeClock& clock) noexcept
	{
		return clock.Now();
	}

	String TimeSpan::Format(const String& format) const
	{
		if (format.IsEmpty())
		{
			return wxTimeSpan::Milliseconds(m_Value).Format(wxDefaultTimeSpanFormat);
		}
		else
		{
			return wxTimeSpan::Milliseconds(m_Value).Format(format);
		}
	}
	String TimeSpan::Format(const Locale& locale, FlagSet<TimeFormatFlag> flags) const
	{
		if (flags & TimeFormatFlag::NoMinutes)
		{
			return Format(wxS("%H"));
		}
		else if (flags & TimeFormatFlag::NoSeconds)
		{
			return Format(wxS("%H:%M"));
		}
		else
		{
			return Format(wxS("%H:%M:%S"));
		}
	}
}
