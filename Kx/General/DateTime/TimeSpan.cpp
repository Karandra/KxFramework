#include "stdafx.h"
#include "TimeSpan.h"
#include "TimeClock.h"
#include "Kx/Localization/Locale.h"

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
	String TimeSpan::Format(const Locale& locale, TimeFormatFlag flags) const
	{
		return locale.FormatTime(wxDateTime().SetMillisecond(m_Value), flags);
	}
}
