#include "stdafx.h"
#include "DateSpan.h"
#include "DateTime.h"

namespace KxFramework
{
	String DateSpan::Format(const String& format) const
	{
		return DateTime().SetDateSpan(*this).Format(format);
	}
	String DateSpan::Format(const Locale& locale, DateFormatFlag flags) const
	{
		return DateTime().SetDateSpan(*this).FormatDate(locale, flags);
	}
}
