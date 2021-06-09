#include "KxfPCH.h"
#include "Format.h"
#include "RegEx.h"
#include "kxf/Log/Common.h"

namespace kxf::Private
{
	String ConvertQtStyleFormat(const String& format)
	{
		RegEx regEx("%(\\d+)");

		String result = format;
		while (regEx.Matches(format))
		{
			regEx.ReplaceAll(result, "{\\1}");
		}
		return result;
	}
	void LogFormatterException(const std::format_error& e)
	{
		Log::Error(e.what());
	}
}
